
#include "lg_string.h"
#include "lg_ngx_session.h"
#include "lg_ngx_network.h"
#include "lg_ngx_uri.h"

void lg_ngx_uri_parse_domain(ngx_http_request_t *r, lg_ngx_uri_t *uri)
{
    ngx_http_headers_in_t *headers_in = &r->headers_in;
    if (!headers_in || !headers_in->server.data || !headers_in->server.len)
	return;

    uri->domain = (char *)headers_in->server.data;
    uri->domain_root = lg_string_url_to_domain(uri->domain);
}

void lg_ngx_uri_parse_domain_s(ngx_http_request_t *r, lg_ngx_uri_t *uri)
{
    if (!uri->domain || !uri->domain_root)
	return;

    if (strcmp(uri->domain, uri->domain_root) == 0)
	return;

    int domain_s_len = uri->domain_root - uri->domain - 1;
    if (domain_s_len <= 0) return;

    uri->domain_s = ngx_palloc(r->pool, domain_s_len + 1);
    if (!uri->domain_s) return;

    memcpy(uri->domain_s, uri->domain, domain_s_len);
    uri->domain_s[domain_s_len] = 0;
}

void lg_ngx_uri_parse_domain_e(ngx_http_request_t *r, lg_ngx_uri_t *uri)
{
    char *s = (char *)r->uri_start;
    char *e = (char *)r->uri_end;

    if ((s == e) || (*s == '/' && (s + 1) == e)) { 
	uri->uri_type = LG_NGX_URI_TYPE_ROOT;
	uri->domain_e = "/";
	return;
    }

    if (strstr(s, ".html")) uri->uri_type = LG_NGX_URI_TYPE_HTML;
    else uri->uri_type = LG_NGX_URI_TYPE_LIST;

    uri->domain_e = (char *)s;
    if (*uri->domain_e == '/') uri->domain_e = s + 1;
    for (; s <= e; s++) {
	if (!(*s == '/' || isalnum(*s))) { // 字母 or 数字 or /
	    *s = 0;
	    return;
	}
    }
}

void lg_ngx_uri_parse_user_agent(ngx_http_request_t *r, lg_ngx_uri_t *uri)
{
    char *user_agent = lg_ngx_network_get_user_agent(r);
    if (user_agent) {

	uri->user_agent = user_agent;

	if (lg_string_strstr(user_agent, "ipad")
		|| lg_string_strstr(user_agent, "iphone")
		|| lg_string_strstr(user_agent, "android")
		|| lg_string_strstr(user_agent, "windows ce")
		|| lg_string_strstr(user_agent, "windows mobile")
	   ) {
	    uri->mobile_type = 1;
	}

	if (lg_string_strstr(user_agent, "spider")
		|| lg_string_strstr(user_agent, "baidu")		// 百度
		|| lg_string_strstr(user_agent, "google")		// 谷歌
		|| lg_string_strstr(user_agent, "soso")
		|| lg_string_strstr(user_agent, "360")			// 360
		|| lg_string_strstr(user_agent, "yahoo")
		|| lg_string_strstr(user_agent, "youdao")
		|| lg_string_strstr(user_agent, "sogou")		// 搜狗
		|| lg_string_strstr(user_agent, "gougou")
		|| lg_string_strstr(user_agent, "yisou")		// 神马
		|| lg_string_strstr(user_agent, "haosou")		// 好搜
		|| lg_string_strstr(user_agent, "bingbot")		// 必应
		|| lg_string_strstr(user_agent, "SemrushBot")		// 
	   ) {
	    uri->search_engine = 1;
	}
    }
}

int lg_ngx_uri_parse(ngx_http_request_t *r, lg_ngx_uri_t *uri)
{
    memset(uri, 0, sizeof(lg_ngx_uri_t));

    lg_ngx_uri_parse_domain(r, uri);
    lg_ngx_uri_parse_domain_s(r, uri);
    lg_ngx_uri_parse_domain_e(r, uri);
    lg_ngx_uri_parse_user_agent(r, uri);

#if 0
    fprintf(stdout, "domain :        %s\n",   uri->domain);
    fprintf(stdout, "domain_root :   %s\n",   uri->domain_root);
    fprintf(stdout, "domain_s :      %s\n",   uri->domain_s);
    fprintf(stdout, "domain_e :      %s\n",   uri->domain_e);
    fprintf(stdout, "uri_type :      %d\n",   uri->uri_type);
    fprintf(stdout, "mobile_type :   %d\n",   uri->mobile_type);
    fprintf(stdout, "search_engine : %d\n",   uri->search_engine);
#endif
    return 0;
}



