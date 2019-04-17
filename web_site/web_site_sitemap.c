
#include "web_site_module.h"

typedef struct {

    int pos_web_site;
    char *domain_root;

    lg_db_t *table;
    char *col_name;

    int page_index;	// 当前页数
    int page_total;	// 总页面数量
    int link_count;	// 每页链接数量

} sitemap_pos_t;

/********************/
/*** sitemap loop ***/
/********************/
void web_site_sitemap_loop(ngx_http_request_t *r,
	lg_ngx_network_t *network,
	template_t *template,
	lg_db_t *config,
	sitemap_pos_t *pos)
{
    int i;
    int link_count = pos->link_count;
    int index = pos->page_index ? pos->page_index * link_count + 1 : 1; // 起始
    for (i = 0; i < link_count; i++) {

	int key = index + i;

	lg_db_t *row = lg_db_find_index_r(pos->table, pos->pos_web_site + key);
	if (!row) return;

	char *keyword = lg_db_to_string(lg_db_find(row, lg_db_data_string(pos->col_name)));
	if (!keyword) return;

#ifdef DOMAIN_MAP
	char href[128];
	sprintf(href, "http://%03d.%s", key, pos->domain_root);
#else
	char href_encode[128];
	domain_map_encode(config, key, href_encode);

	char href[128];
	sprintf(href, "http://%s.%s", href_encode, pos->domain_root);
#endif
	lg_ngx_network_str_add(r, network, "<li><a href=\"");
	lg_ngx_network_str_add(r, network, href);
	lg_ngx_network_str_add(r, network, "\">");
	lg_ngx_network_str_add(r, network, keyword);
	lg_ngx_network_str_add(r, network, "</a></li>\r\n");
    }
}

/********************/
/*** sitemap list ***/
/********************/
void web_site_sitemap_list(ngx_http_request_t *r,
	lg_ngx_network_t *network,
	template_t *template,
	lg_db_t *config,
	sitemap_pos_t *pos)
{
    int i;
    char buf[1024];
    for (i = 0; i <= pos->page_total; i++) {
	sprintf(buf, "<a href=\"http://www.%s/sitemap/%d.html\" title=\"第%d页\">%d</a>\r\n", pos->domain_root, i, i, i);
	lg_ngx_network_str_add(r, network, buf);
    }
}

int web_site_sitemap(ngx_http_request_t *r,
	lg_ngx_network_t *network,
	template_t *template,
	lg_db_t *config,
	pos_t *display_pos)
{

    // fprintf(stdout, "web_site_sitemap: 1\n");

    sitemap_pos_t *pos = ngx_palloc(r->pool, sizeof(sitemap_pos_t));
    if (!pos) return lg_ngx_network_not_found(r);

    pos->pos_web_site = display_pos->pos_web_site;
    pos->domain_root = display_pos->domain_root;
    pos->page_index = atoi(display_pos->uri->domain_e ? display_pos->uri->domain_e : "0");

    char *table_name = lg_db_to_string(lg_db_find(config, lg_db_data_string("sitemap_table")));
    if (!table_name) return lg_ngx_network_not_found(r);

    // table
    pos->table = lg_db_find(get_db(), lg_db_data_string(table_name));
    if (!pos->table || pos->table->child_total <= 0) return lg_ngx_network_not_found(r);
    // fprintf(stdout, "web_site_sitemap: 2\n");

    // col_name
    pos->col_name = lg_db_to_string(lg_db_find(config, lg_db_data_string("sitemap_col")));
    if (!pos->col_name) return lg_ngx_network_not_found(r);
    // fprintf(stdout, "web_site_sitemap: 3\n");

    // link_count
    pos->link_count = lg_db_to_number(lg_db_find(config, lg_db_data_string("sitemap_count")));
    if (pos->link_count <= 0) return lg_ngx_network_not_found(r);
    // fprintf(stdout, "web_site_sitemap: 4\n");

    // page_total
    pos->page_total = pos->table->child_total / pos->link_count;
    if (pos->page_index > pos->page_total) return lg_ngx_network_not_found(r);

#if 0
    fprintf(stdout, "domain_e   : %s\n", display_pos->uri->domain_e);
    fprintf(stdout, "page_index : %d\n", pos->page_index);
    fprintf(stdout, "page_total : %d\n", pos->page_total);
    fprintf(stdout, "link_count : %d\n", pos->link_count);
    fprintf(stdout, "table_name : %s\n", table_name);
    fprintf(stdout, "col_name   : %s\n", pos->col_name);
    fprintf(stdout, "==================================\n");
#endif

    char buf[1024];
    sprintf(buf, "<!DOCTYPE HTML>\r\n"
	    "<html>\r\n"
	    "<head>\r\n"
	    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\r\n"
	    "<meta name=\"generator\" content=\"Baidu SiteMap Generator\">\r\n"
	    "<title>网站地图 - 第%d页</title>\r\n"
	    "</head>\r\n"
	    "<body>\r\n"
	    "<h3>网页列表地址</h3>\r\n"
	    "<ul>\r\n"
	    , pos->page_index);

    lg_ngx_network_str_add(r, network, buf);
    web_site_sitemap_loop(r, network, template, config, pos);
    lg_ngx_network_str_add(r, network, "</ul>\r\n");
    web_site_sitemap_list(r, network, template, config, pos);
    lg_ngx_network_str_add(r, network, "</body>\r\n</html>\r\n");

    return lg_ngx_network_send(r, network);
}




