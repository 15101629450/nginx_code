
#include "web_site_module.h"

#define POS_RAND 1024

lg_db_t *lg_db_find_index(lg_db_t *db, int index)
{
    return lg_db_find(db, lg_db_data_number(index));
}

lg_db_t *lg_db_find_index_r(lg_db_t *db, int index)
{
    if (!db || db->child_total <= 0 || index <= 0) return NULL;
    if (index <= db->child_total) return lg_db_find_index(db, index);

    index = index % db->child_total;
    if (index == 0) index = db->child_total;
    return lg_db_find_index(db, index);
}

lg_db_t *lg_db_find_string(lg_db_t *db, char *key)
{
    return lg_db_find(db, lg_db_data_string(key));
}

void web_site_display_table_this(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    lg_db_t *table	= lg_db_find_string(get_db(), node->table_name);
    lg_db_t *row	= lg_db_find_index_r(table, pos->pos_web_site + pos->key);
    lg_db_t *col	= lg_db_find_string(row, node->col_name);
    lg_ngx_network_str_add(r, network, lg_db_to_string(col));
}

void web_site_display_table_next(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    lg_db_t *table	= lg_db_find_string(get_db(), node->table_name);
    lg_db_t *row	= lg_db_find_index_r(table, pos->pos_web_site + (++pos->pos_next));
    lg_db_t *col	= lg_db_find_string(row, node->col_name);
    lg_ngx_network_str_add(r, network, lg_db_to_string(col));
}

void web_site_display_table_rand(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    lg_db_t *table	= lg_db_find_string(get_db(), node->table_name);
    lg_db_t *row	= lg_db_find_index_r(table, pos->pos_web_site + pos->key + rand() % POS_RAND);
    lg_db_t *col	= lg_db_find_string(row, node->col_name);
    lg_ngx_network_str_add(r, network, lg_db_to_string(col));
}

void web_site_display_table_loop(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
}

void web_site_display_table_link(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    lg_db_t *table = lg_db_find_string(get_db(), node->table_name);
    lg_db_t *row = lg_db_find_index_r(table, pos->pos_web_site + pos->pos_href_cur);
    lg_db_t *col = lg_db_find_string(row, node->col_name);
    lg_ngx_network_str_add(r, network, lg_db_to_string(col));
}

void web_site_display_link_href(ngx_http_request_t *r, lg_ngx_network_t *network, lg_db_t *config, pos_t *pos)
{
    char buf[1024];
#ifdef DOMAIN_MAP
    if (pos->pos_href_s && pos->pos_href_e)
	sprintf(buf, "http://%04d.%s/%04d.html", pos->pos_href_s, pos->domain_root, pos->pos_href_e);
    else if (pos->pos_href_s)
	sprintf(buf, "http://%04d.%s/", pos->pos_href_s, pos->domain_root);
    else if (pos->pos_href_e)
	sprintf(buf, "http://%s/%04d.html", pos->domain_root, pos->pos_href_e);
    else
	sprintf(buf, "http://%s", pos->uri->domain);
#else
    char pos_href_s[128];
    char pos_href_e[128];
    domain_map_encode(config, pos->pos_href_s, pos_href_s);
    domain_map_encode(config, pos->pos_href_e, pos_href_e);
    // fprintf(stdout, "pos_href_s : [%d][%s]\n", pos->pos_href_s, pos_href_s);
    // fprintf(stdout, "pos_href_e : [%d][%s]\n", pos->pos_href_e, pos_href_e);

    if (pos->pos_href_s && pos->pos_href_e)
	sprintf(buf, "http://%s.%s/%s.html", pos_href_s, pos->domain_root, pos_href_e);
    else if (pos->pos_href_s)
	sprintf(buf, "http://%s.%s/", pos_href_s, pos->domain_root);
    else if (pos->pos_href_e)
	sprintf(buf, "http://%s/%s.html", pos->domain_root, pos_href_e);
    else
	sprintf(buf, "http://%s", pos->uri->domain);
#endif

    pos->pos_copy_s = pos->pos_href_s;
    pos->pos_copy_e = pos->pos_href_e;
    pos->pos_copy_cur = pos->pos_href_cur;

    lg_ngx_network_str_add(r, network, buf);
}

/* link this */
void web_site_display_link_this_this(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = pos->domain_s;
    pos->pos_href_e = pos->domain_e;
    pos->pos_href_cur = pos->pos_href_e ? pos->pos_href_e : pos->pos_href_s;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_this_next(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = pos->domain_s;
    pos->pos_href_e = ++pos->pos_next;
    pos->pos_href_cur = pos->pos_href_e;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_this_rand(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = pos->domain_s;
    pos->pos_href_e = rand() % POS_RAND + pos->key;
    pos->pos_href_cur = pos->pos_href_e;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_this_jump(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = pos->domain_s;
    pos->pos_href_e = ++pos->pos_link_jump + pos->pos_next;
    pos->pos_href_cur = pos->pos_href_e;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_this_null(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = pos->domain_s;
    pos->pos_href_e = 0;
    pos->pos_href_cur = pos->pos_href_s;
    web_site_display_link_href(r, network, config, pos);
}

/* link next */
void web_site_display_link_next_this(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = ++pos->pos_next;
    pos->pos_href_e = pos->domain_e;
    pos->pos_href_cur = pos->pos_href_e ? pos->pos_href_e : pos->pos_href_s;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_next_next(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = ++pos->pos_next;
    pos->pos_href_e = pos->pos_href_s;
    pos->pos_href_cur = pos->pos_href_s;
    web_site_display_link_href(r, network, config, pos);
}
void web_site_display_link_next_rand(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = ++pos->pos_next;
    pos->pos_href_e = rand() % POS_RAND + pos->key;
    pos->pos_href_cur = pos->pos_href_e;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_next_jump(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = ++pos->pos_next;
    pos->pos_href_e = ++pos->pos_link_jump + pos->pos_next;
    pos->pos_href_cur = pos->pos_href_e;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_next_null(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = ++pos->pos_next;
    pos->pos_href_e = 0;
    pos->pos_href_cur = pos->pos_href_s;
    web_site_display_link_href(r, network, config, pos);
}

/* link rand */
void web_site_display_link_rand_this(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = rand() % POS_RAND + pos->key;
    pos->pos_href_e = pos->domain_e;
    pos->pos_href_cur = pos->pos_href_e ? pos->pos_href_e : pos->pos_href_s;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_rand_next(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = rand() % POS_RAND + pos->key;
    pos->pos_href_e = ++pos->pos_next;
    pos->pos_href_cur = pos->pos_href_e;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_rand_rand(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = rand() % POS_RAND + pos->key;
    pos->pos_href_e = rand() % POS_RAND + pos->key;
    pos->pos_href_cur = pos->pos_href_e;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_rand_jump(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = rand() % POS_RAND + pos->key;
    pos->pos_href_e = ++pos->pos_link_jump + pos->pos_next;
    pos->pos_href_cur = pos->pos_href_e;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_rand_null(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = rand() % POS_RAND + pos->key;
    pos->pos_href_e = 0;
    pos->pos_href_cur = pos->pos_href_s;
    return web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_jump_this(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = ++pos->pos_link_jump + pos->pos_next;
    pos->pos_href_e = pos->domain_e;
    pos->pos_href_cur = pos->pos_href_e ? pos->pos_href_e : pos->pos_href_s;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_jump_next(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = ++pos->pos_link_jump + pos->pos_next;
    pos->pos_href_e = ++pos->pos_next;
    pos->pos_href_cur = pos->pos_href_e;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_jump_rand(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = ++pos->pos_link_jump + pos->pos_next;
    pos->pos_href_e = rand() % POS_RAND + pos->key;
    pos->pos_href_cur = pos->pos_href_e;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_jump_jump(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = ++pos->pos_link_jump + pos->pos_next;
    pos->pos_href_e = ++pos->pos_link_jump + pos->pos_next;
    pos->pos_href_cur = pos->pos_href_e;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_jump_null(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s = ++pos->pos_link_jump + pos->pos_next;
    pos->pos_href_e = 0;
    pos->pos_href_cur = pos->pos_href_s;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_link_copy(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    pos->pos_href_s =	pos->pos_copy_s;
    pos->pos_href_e =	pos->pos_copy_e;
    pos->pos_href_cur =	pos->pos_copy_cur;
    web_site_display_link_href(r, network, config, pos);
}

void web_site_display_search_engine(ngx_http_request_t *r, lg_ngx_network_t *network, template_node_t *node, lg_db_t *config, pos_t *pos)
{
    if (pos->uri->search_engine)
	lg_ngx_network_str_add(r, network, "true");
    else
	lg_ngx_network_str_add(r, network, "false");
}

void web_site_display_match(ngx_http_request_t *r,
	lg_ngx_network_t *network,
	template_node_t *node,
	lg_db_t *config,
	pos_t *pos)
{

    switch(node->type) {
	case HTML : lg_ngx_network_str_add(r, network, node->html); return;
		    /*****************/
		    /***   TABLE   ***/
		    /*****************/
	case TABLE_THIS : return web_site_display_table_this(r, network, node, config, pos);
	case TABLE_NEXT : return web_site_display_table_next(r, network, node, config, pos);
	case TABLE_RAND : return web_site_display_table_rand(r, network, node, config, pos);
	case TABLE_LOOP : return web_site_display_table_loop(r, network, node, config, pos);
	case TABLE_LINK : return web_site_display_table_link(r, network, node, config, pos);
			  /*****************/
			  /*** LINK THIS ***/
			  /*****************/
	case LINK_THIS_THIS : return web_site_display_link_this_this(r, network, node, config, pos);
	case LINK_THIS_NEXT : return web_site_display_link_this_next(r, network, node, config, pos);
	case LINK_THIS_RAND : return web_site_display_link_this_rand(r, network, node, config, pos);
	case LINK_THIS_JUMP: return web_site_display_link_this_jump(r, network, node, config, pos);
	case LINK_THIS_NULL : return web_site_display_link_this_null(r, network, node, config, pos);
			      /*****************/
			      /*** LINK NEXT ***/
			      /*****************/
	case LINK_NEXT_THIS : return web_site_display_link_next_this(r, network, node, config, pos);
	case LINK_NEXT_NEXT : return web_site_display_link_next_next(r, network, node, config, pos);
	case LINK_NEXT_RAND : return web_site_display_link_next_rand(r, network, node, config, pos);
	case LINK_NEXT_JUMP : return web_site_display_link_next_jump(r, network, node, config, pos);
	case LINK_NEXT_NULL : return web_site_display_link_next_null(r, network, node, config, pos);
			      /*****************/
			      /*** LINK RAND ***/
			      /*****************/
	case LINK_RAND_THIS : return web_site_display_link_rand_this(r, network, node, config, pos);
	case LINK_RAND_NEXT : return web_site_display_link_rand_next(r, network, node, config, pos);
	case LINK_RAND_RAND : return web_site_display_link_rand_rand(r, network, node, config, pos);
	case LINK_RAND_JUMP : return web_site_display_link_rand_jump(r, network, node, config, pos);
	case LINK_RAND_NULL : return web_site_display_link_rand_null(r, network, node, config, pos);
			      /*****************/
			      /*** LINK JUMP ***/
			      /*****************/
	case LINK_JUMP_THIS : return web_site_display_link_jump_this(r, network, node, config, pos);
	case LINK_JUMP_NEXT : return web_site_display_link_jump_next(r, network, node, config, pos);
	case LINK_JUMP_RAND : return web_site_display_link_jump_rand(r, network, node, config, pos);
	case LINK_JUMP_JUMP : return web_site_display_link_jump_jump(r, network, node, config, pos);
	case LINK_JUMP_NULL : return web_site_display_link_jump_null(r, network, node, config, pos);
			      /*****************/
			      /*** LINK COPY ***/
			      /*****************/
	case LINK_COPY : return web_site_display_link_copy(r, network, node, config, pos);
			 /*********************/
			 /*** SEARCH_ENGINE ***/
			 /*********************/
	case SEARCH_ENGINE : return web_site_display_search_engine(r, network, node, config, pos);
    }
}

int web_site_display(ngx_http_request_t *r,
	lg_ngx_network_t *network,
	template_t *template,
	lg_db_t *config,
	pos_t *pos)
{

    lg_list_t *p = NULL;
    lg_list_for_each(p, &template->head) {
	web_site_display_match(r, network, (template_node_t *)p, config, pos);
    }

    return lg_ngx_network_send(r, network);
}




