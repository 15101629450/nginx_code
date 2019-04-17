
#ifndef __LG_NGX_URI_H__
#define __LG_NGX_URI_H__

#include <nginx.h>
#include <ngx_core.h>
#include <ngx_http.h>

#define LG_NGX_URI_TYPE_ROOT	0x01
#define LG_NGX_URI_TYPE_LIST	0x02
#define LG_NGX_URI_TYPE_HTML	0x03

typedef struct {

    char *domain;	// 001.xxx.com/002 => 001.xxx.com
    char *domain_root;	// 001.xxx.com/002 => xxx.com
    char *domain_s;	// 001.xxx.com/002 => 001
    char *domain_e;	// 001.xxx.com/002 => 002

    int uri_type;	// 1:root 2:list 3:html
    int mobile_type;	// 0:pc 1:mobile
    int search_engine;	// 0:null 1:search engine

    char *user_agent;

} lg_ngx_uri_t;

extern int lg_ngx_uri_parse(ngx_http_request_t *r, lg_ngx_uri_t *uri);

#endif


