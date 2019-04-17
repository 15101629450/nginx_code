
#include "web_site_module.h"

void web_site_uri_parse(ngx_http_request_t *r, lg_ngx_uri_t *uri)
{
    if (strstr(uri->domain_e, "sitemap")) uri->uri_type = 100;
    // else uri->uri_type = (uri->uri_type == LG_NGX_URI_TYPE_ROOT) ? 1 : 2;

    char *save = NULL;
    char *str = strtok_r(uri->domain_e, "/", &save);

    uri->domain_e = "/";
    if (uri->uri_type != 100) uri->uri_type = 1;

    while(str) {
	uri->domain_e = str;
	if (uri->uri_type != 100) uri->uri_type = 2;
	str = strtok_r(NULL, "/", &save);
    }

    // push baidu
    char addr[128] = {0};
    lg_ngx_network_get_ipaddr(r, addr);
    if (strcmp(addr, "119.253.46.110") == 0)
	uri->search_engine = 1;
}

template_t *web_site_page_template_select(lg_db_t *config, int template_type)
{
    char *template_name = NULL;
    switch(template_type) {

	case 1 : template_name = "template_home";
		 break;
	case 2 : template_name = "template_content";
		 break;
	case 3 : template_name = "template_sitemap";
		 break;
	default : return NULL;
    }

    int template_index = lg_db_to_number(lg_db_find(config, lg_db_data_string(template_name)));
    return lg_db_to_struct(lg_db_find_args(get_db(),
		lg_db_data_string("t_template"),
		lg_db_data_number(template_index),
		lg_db_data_null()));
}

int web_site_page(ngx_http_request_t *r)
{
    // network init
    lg_ngx_network_t network;
    memset(&network, 0, sizeof(lg_ngx_network_t));
    network.status = NGX_HTTP_OK;
    network.content_type.data = (u_char *)"text/html; charset=utf-8";
    network.content_type.len = strlen("text/html; charset=utf-8");

    // get URI
    lg_ngx_uri_t *uri = ngx_palloc(r->pool, sizeof(lg_ngx_uri_t));
    if (!uri) lg_ngx_network_not_found(r);
    memset(uri, 0, sizeof(lg_ngx_uri_t));

    if (lg_ngx_uri_parse(r, uri))
	return lg_ngx_network_not_found(r);

    if (strstr(uri->domain_e, "robots"))
	return lg_ngx_network_not_found(r);

    if (strstr(uri->domain_e, "favicon"))
	return lg_ngx_network_not_found(r);

    web_site_uri_parse(r, uri);

    // domain -> config
    lg_db_t *config = lg_db_find_args(get_db(),
	    lg_db_data_string(TABLE_CONFIG),
	    lg_db_data_string(uri->domain_root),
	    lg_db_data_null());
    if (!config) lg_ngx_network_not_found(r);

    pos_t *pos = ngx_palloc(r->pool, sizeof(pos_t));
    if (!pos) lg_ngx_network_not_found(r);
    memset(pos, 0, sizeof(pos_t));

    pos->domain_root = uri->domain_root;
#ifdef DOMAIN_MAP
    pos->domain_s = atoi(uri->domain_s ? uri->domain_s : "0");
    pos->domain_e = atoi(uri->domain_e ? uri->domain_e : "0");
#else
    pos->domain_s = domain_map_decode(config, uri->domain_s);
    pos->domain_e = domain_map_decode(config, uri->domain_e);
#endif
    lg_list_init(&pos->pos_null);
    pos->uri = uri;

    // pos_web_site
    pos->pos_web_site = lg_db_to_number(lg_db_find(config, lg_db_data_string("pos_web_site")));
    if (pos->pos_web_site <= 0) return lg_ngx_network_not_found(r);

    // pos_web_site
    pos->pos_link_jump = lg_db_to_number(lg_db_find(config, lg_db_data_string("pos_link_jump")));
    if (pos->pos_link_jump <= 0) return lg_ngx_network_not_found(r);

    // key
    pos->key = pos->domain_e ? pos->domain_e : pos->domain_s;
    pos->key = pos->key ? pos->key : 1;
    pos->pos_next = pos->key;
    pos->pos_href_cur = pos->key;

#if 0
    fprintf(stdout, "[uri]domain         : %s\n", uri->domain);
    fprintf(stdout, "[uri]uri_type       : %d\n", uri->uri_type);
    fprintf(stdout, "[uri]mobile_type    : %d\n", uri->mobile_type);
    fprintf(stdout, "[uri]search_engine  : %d\n", uri->search_engine);
    fprintf(stdout, "[uri]domain_root    : %s\n", uri->domain_root);
    fprintf(stdout, "[uri]domain_s       : %s\n", uri->domain_s);
    fprintf(stdout, "[uri]domain_e       : %s\n", uri->domain_e);
    fprintf(stdout, "[pos]domain_s       : %d\n", pos->domain_s);
    fprintf(stdout, "[pos]domain_e       : %d\n", pos->domain_e);
    fprintf(stdout, "[pos]key            : %d\n", pos->key);
    fprintf(stdout, "[pos]pos_web_site   : %d\n", pos->pos_web_site);
    fprintf(stdout, "[pos]pos_link_jump  : %d\n", pos->pos_link_jump);
    fprintf(stdout, "[pos]pos_next       : %d\n", pos->pos_next);
    fprintf(stdout, "[pos]pos_href_s     : %d\n", pos->pos_href_s);
    fprintf(stdout, "[pos]pos_href_e     : %d\n", pos->pos_href_e);
    fprintf(stdout, "[pos]pos_href_cur   : %d\n", pos->pos_href_cur);
    fprintf(stdout, "[pos]pos_copy_s     : %d\n", pos->pos_copy_s);
    fprintf(stdout, "[pos]pos_copy_e     : %d\n", pos->pos_copy_e);
    fprintf(stdout, "[pos]pos_copy_cur   : %d\n", pos->pos_copy_cur);
    fprintf(stdout, "==================================\n");
#endif

    // fprintf(stdout, "[search_engine][%d]\n", uri->search_engine);
    // fprintf(stdout, "[user_agent]%s\n", uri->user_agent);

    if (uri->uri_type == 100) {

	// template_t *template = web_site_page_template_select(config, uri->uri_type);
	// if (!template) return lg_ngx_network_not_found(r);
	// return web_site_sitemap(r, &network, template, config, pos);
	return web_site_sitemap(r, &network, NULL, config, pos);
    }

    template_t *template = web_site_page_template_select(config, uri->uri_type);
    if (!template) return lg_ngx_network_not_found(r);

    return web_site_display(r, &network, template, config, pos);
    return lg_ngx_network_str_send(r, NULL, "ok");
}





