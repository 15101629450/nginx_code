
#include <nginx.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include "lg_ngx_network.h"

// Location	http://www.aaa.com/

static char *demo_module_command(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

/* Commands */
static ngx_command_t  demo_module_commands[] = {
    { ngx_string("demo_module"),
	NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS,
	demo_module_command,
	NGX_HTTP_LOC_CONF_OFFSET,
	0,
	NULL },

    ngx_null_command
};

static ngx_http_module_t demo_module_ctx = {
    NULL,			/* preconfiguration */
    NULL,			/* postconfiguration */
    NULL,			/* create main configuration */
    NULL,			/* init main configuration */
    NULL,			/* create server configuration */
    NULL,			/* merge server configuration */
    NULL,			/* create location configuration */
    NULL			/* merge location configuration */
};

/* hook */
ngx_module_t demo_module = {
    NGX_MODULE_V1,
    &demo_module_ctx,		/* module context */
    demo_module_commands,	/* module directives */
    NGX_HTTP_MODULE,		/* module type */
    NULL,			/* init master */
    NULL,			/* init module */
    NULL,			/* init process */
    NULL,			/* init thread */
    NULL,			/* exit thread */
    NULL,			/* exit process */
    NULL,			/* exit master */
    NGX_MODULE_V1_PADDING
};

static void demo_module_handler_post(ngx_http_request_t *r)
{

}

static ngx_int_t demo_module_handler_get(ngx_http_request_t *r)
{
    char *k = "Location";
    char *v = "http://www.bbb.com";
    lg_ngx_network_headers_add(r, k, v);

    lg_ngx_network_t *p = ngx_pcalloc(r->pool, sizeof(lg_ngx_network_t));
    p->status = 302;

    return lg_ngx_network_buffer_send(r, p, "302", 3);
}

static ngx_int_t demo_module_handler(ngx_http_request_t *r)
{

    if (r->method == NGX_HTTP_GET) {

	return demo_module_handler_get(r);

    } else if (r->method == NGX_HTTP_POST) {

	ngx_int_t rc = ngx_http_read_client_request_body(r, demo_module_handler_post);
	if (rc >= NGX_HTTP_SPECIAL_RESPONSE)
	    return rc; 

	return NGX_DONE;
    }

    return NGX_ERROR;
}

static char *demo_module_command(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *loc_conf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    loc_conf->handler = demo_module_handler;
    return NGX_CONF_OK;
}



