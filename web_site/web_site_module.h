
#ifndef	__WEB_SITE_MODULE_H__
#define __WEB_SITE_MODULE_H__

#include <nginx.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include "lg_ngx_uri.h"
#include "lg_ngx_network.h"
#include "lg_ngx_session.h"

#include "lg_list.h"
#include "lg_tree.h"
#include "lg_string.h"
#include "lg_mysql.h"
#include "lg_db.h"

/****************/
/*** module ***/
/****************/

typedef struct {

    int type;
    ngx_str_t web_site_mysql_host;
    ngx_str_t web_site_mysql_base;
    ngx_str_t web_site_mysql_user;
    ngx_str_t web_site_mysql_pass;

} web_site_module_conf_t;

extern char *web_site_init(web_site_module_conf_t *conf);
extern ngx_int_t web_site_work_init(ngx_cycle_t *cycle);
extern lg_db_t *get_db(void);

/****************/
/*** template ***/
/****************/

#define HTML			100

#define TABLE_THIS		1
#define TABLE_NEXT		2
#define TABLE_RAND		3
#define TABLE_LOOP		4
#define TABLE_LINK		5

#define LINK_THIS_THIS		6
#define LINK_THIS_NEXT		7
#define LINK_THIS_RAND		8
#define LINK_THIS_JUMP		9
#define LINK_THIS_NULL		10

#define LINK_NEXT_THIS		11
#define LINK_NEXT_NEXT		12
#define LINK_NEXT_RAND		13
#define LINK_NEXT_JUMP		14
#define LINK_NEXT_NULL		15

#define LINK_RAND_THIS		16
#define LINK_RAND_NEXT		17
#define LINK_RAND_RAND		18
#define LINK_RAND_JUMP		19
#define LINK_RAND_NULL		20

#define LINK_JUMP_THIS		21
#define LINK_JUMP_NEXT		22
#define LINK_JUMP_RAND		23
#define LINK_JUMP_JUMP		24
#define LINK_JUMP_NULL		25

#define LINK_COPY		26

#define SEARCH_ENGINE		27

#define ARRAY_SIZE		27
#define TABLE_CONFIG		"t_config"

typedef struct __template_node_t template_node_t;
struct __template_node_t {

    lg_list_t next;

    int type;
    char *html;
    char *table_name;
    char *col_name;
};

typedef struct __template_t template_t;
struct __template_t {

    lg_list_t head;
    int node_type_count[ARRAY_SIZE + 1];
    char *buf;
};

extern template_t *web_site_template_create(char *buf);
extern void web_site_template_display(template_t *template);

/******************/
/*** domain map ***/
/******************/

// #define DOMAIN_MAP
void domain_map_encode(lg_db_t *db, int src, char *dest);
int domain_map_decode(lg_db_t *db, char *src);

/************************/
/*** page and display ***/
/************************/

typedef struct {

    lg_list_t next;
    char *name;
    int pos;

} pos_null_t;

typedef struct {

    int domain_s;
    int domain_e;
    char *domain_root;

    lg_list_t pos_null;
    lg_ngx_uri_t *uri;

    int key;
    int pos_web_site;
    int pos_link_jump;
    int pos_next;

    int pos_href_s;
    int pos_href_e;
    int pos_href_cur;

    int pos_copy_s;
    int pos_copy_e;
    int pos_copy_cur;

} pos_t;

lg_db_t *lg_db_find_index(lg_db_t *db, int index);
lg_db_t *lg_db_find_index_r(lg_db_t *db, int index);
lg_db_t *lg_db_find_string(lg_db_t *db, char *key);

extern int web_site_page(ngx_http_request_t *r);
extern int web_site_display(ngx_http_request_t *r, lg_ngx_network_t *network, template_t *template, lg_db_t *config, pos_t *pos);
extern int web_site_sitemap(ngx_http_request_t *r, lg_ngx_network_t *network, template_t *template, lg_db_t *config, pos_t *pos);

/************/
/*** demo ***/
/************/
char *web_site_demo();

#endif


