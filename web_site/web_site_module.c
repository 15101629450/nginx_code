
#include "web_site_module.h"

static void *ngx_web_site_module_create(ngx_conf_t *cf);
static char *ngx_web_site_module_command(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char *ngx_web_site_module_init(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t ngx_web_site_module_worker(ngx_cycle_t *cycle);
static ngx_int_t ngx_web_site_module_handler(ngx_http_request_t *r);

/* Commands */
static ngx_command_t  ngx_web_site_module_commands[] = {
    {
	ngx_string("web_site_module"),
	NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS,
	ngx_web_site_module_command,
	NGX_HTTP_LOC_CONF_OFFSET,
	0,
	NULL },
    {
	ngx_string("web_site_mysql_host"),
	NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
	ngx_conf_set_str_slot,
	NGX_HTTP_LOC_CONF_OFFSET,
	offsetof(web_site_module_conf_t, web_site_mysql_host),
	NULL },
    {
	ngx_string("web_site_mysql_base"),
	NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
	ngx_conf_set_str_slot,
	NGX_HTTP_LOC_CONF_OFFSET,
	offsetof(web_site_module_conf_t, web_site_mysql_base),
	NULL },
    {
	ngx_string("web_site_mysql_user"),
	NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
	ngx_conf_set_str_slot,
	NGX_HTTP_LOC_CONF_OFFSET,
	offsetof(web_site_module_conf_t, web_site_mysql_user),
	NULL },
    {
	ngx_string("web_site_mysql_pass"),
	NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
	ngx_conf_set_str_slot,
	NGX_HTTP_LOC_CONF_OFFSET,
	offsetof(web_site_module_conf_t, web_site_mysql_pass),
	NULL },

    ngx_null_command
};

/* http module */
static ngx_http_module_t ngx_web_site_module_ctx = {
    NULL,				/* preconfiguration */
    NULL,				/* postconfiguration */
    NULL,				/* create main configuration */
    NULL,				/* init main configuration */
    NULL,				/* create server configuration */
    NULL,				/* merge server configuration */
    ngx_web_site_module_create,		/* create location configuration */
    ngx_web_site_module_init		/* merge location configuration */
};

/* hook */
ngx_module_t web_site_module = {

    NGX_MODULE_V1,
    &ngx_web_site_module_ctx,		/* module context */
    ngx_web_site_module_commands,	/* module directives */
    NGX_HTTP_MODULE,			/* module type */
    NULL,				/* init master */
    NULL,				/* init module */
    ngx_web_site_module_worker,		/* init process */
    NULL,				/* init thread */
    NULL,				/* exit thread */
    NULL,				/* exit process */
    NULL,				/* exit master */
    NGX_MODULE_V1_PADDING
};

static void *ngx_web_site_module_create(ngx_conf_t *cf)
{
    web_site_module_conf_t *conf = ngx_pcalloc(cf->pool, sizeof(web_site_module_conf_t));
    if (conf) return conf;

    return NGX_CONF_ERROR;
}

static char *ngx_web_site_module_command(ngx_conf_t *cf, ngx_command_t *cmd, void *location_conf)
{
    ngx_http_core_loc_conf_t *core_conf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    core_conf->handler = ngx_web_site_module_handler;

    web_site_module_conf_t *conf = (web_site_module_conf_t *)location_conf;
    conf->type = 0x01;

    return NGX_CONF_OK;
}

static char *ngx_web_site_module_init(ngx_conf_t *cf, void *parent, void *child)
{
    web_site_module_conf_t *conf = ngx_http_conf_get_module_loc_conf(cf, web_site_module);
    if (conf->type) return web_site_init(conf);
    return NGX_CONF_OK;
}

static ngx_int_t ngx_web_site_module_worker(ngx_cycle_t *cycle)
{
    return web_site_work_init(cycle);
}

static ngx_int_t ngx_web_site_module_handler(ngx_http_request_t *r)
{
    if (r->method != NGX_HTTP_GET)
	return NGX_ERROR;

    return web_site_page(r);
}



