
#include "web_site_module.h"

/*** mysql conf ***/
char *mysql_host = NULL;
char *mysql_base = NULL;
char *mysql_user = NULL;
char *mysql_pass = NULL;

/*** db root ***/
lg_db_t *g_db = NULL;
lg_db_t *get_db(void){ return g_db;}

void web_site_table_load_call(int col_len, char **col_name, int *col_type, char **data, void *p)
{
    lg_db_t *db = (lg_db_t *)p;
    lg_db_t *node = col_type[0] == MYSQL_TYPE_LONG
	? lg_db_add(db, lg_db_data_number(lg_string_to_number(data[0])), lg_db_data_null())
	: lg_db_add(db, lg_db_data_string(data[0]), lg_db_data_null());
    if (!node) return;

    int i;
    for (i = 1; i < col_len; i++) {

	if (strcmp(col_name[i], "loop") == 0 || strcmp(col_name[i], "loops") == 0) {
	    lg_db_add(node,
		    lg_db_data_string(data[i]),
		    lg_db_data_null());

	} else if (col_type[i] == MYSQL_TYPE_LONG) {
	    lg_db_add(node,
		    lg_db_data_string(col_name[i]),
		    lg_db_data_number(lg_string_to_number(data[i])));
	} else {
	    lg_db_add(node,
		    lg_db_data_string(col_name[i]),
		    lg_db_data_string(data[i]));
	}
    }
}

lg_db_t *web_site_table_load(lg_db_t *db, char *table)
{
    if (!table) return NULL;

    lg_db_t *node = lg_db_add(db, lg_db_data_string(table), lg_db_data_null());
    if (!node) return NULL;

    char sql[4096];
    sprintf(sql, "select * from %s", table);
    if (lg_mysql_select(mysql_host, mysql_base, mysql_user, mysql_pass, sql, web_site_table_load_call, node))
	return NULL;

    return node;
}

lg_db_t *web_site_db_create(void)
{

    lg_db_t *db = lg_db_create();
    if (!db) return NULL;

    lg_db_t *t_config = web_site_table_load(db, "t_config");
    if (!t_config) {
	lg_db_free(db);
	return NULL;
    }

    lg_db_t *t_template = web_site_table_load(db, "t_template");
    if (!t_template) {
	lg_db_free(db);
	return NULL;
    }

    lg_list_t *p = NULL;
    lg_list_for_each(p, &t_template->child_list) {
	lg_db_t *node = lg_list_entry(p, lg_db_t, next_list);
	lg_db_t *template = lg_db_find(node, lg_db_data_string("template"));
	if (template) node->m_struct = web_site_template_create(template->value.string);
    }

    lg_db_t *t_table = web_site_table_load(db, "t_table");
    if (!t_table) {
	lg_db_free(db);
	return NULL;
    }

    lg_list_for_each(p, &t_table->child_list) {
	lg_db_t *node = lg_list_entry(p, lg_db_t, next_list);
	web_site_table_load(db, node->key.string);
    }

    // lg_db_display(db);
    return db;
}

char *web_site_init(web_site_module_conf_t *conf)
{
    mysql_host = (char *)conf->web_site_mysql_host.data;
    mysql_base = (char *)conf->web_site_mysql_base.data;
    mysql_user = (char *)conf->web_site_mysql_user.data;
    mysql_pass = (char *)conf->web_site_mysql_pass.data;

    // g_db = web_site_db_create();
    // return web_site_demo();
    return NGX_CONF_OK;
    return NGX_CONF_ERROR;
}

ngx_int_t web_site_work_init(ngx_cycle_t *cycle)
{
    g_db = web_site_db_create();
    fprintf(stdout, "pid=%d g_db=%p\n", getpid(), g_db);
    return 0;
}




