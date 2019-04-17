
#include "web_site_module.h"

char *web_site_demo()
{
    // domain -> config
    lg_db_t *config = lg_db_find_args(get_db(),
	    lg_db_data_string(TABLE_CONFIG),
	    lg_db_data_string("ccc.com"),
	    lg_db_data_null());

    char pos_href_s[128];
    char pos_href_e[128];
    domain_map_encode(config, 0, pos_href_s);
    domain_map_encode(config, 0, pos_href_e);

    fprintf(stdout, "pos_href_s : [%s]\n", pos_href_s);
    fprintf(stdout, "pos_href_e : [%s]\n", pos_href_e);

    return NGX_CONF_OK;
    return NGX_CONF_ERROR;
}




