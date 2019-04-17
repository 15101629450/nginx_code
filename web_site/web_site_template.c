
#include "web_site_module.h"

typedef struct {
    char *cmp;
    int len;
    int type;
} template_type_t;
static template_type_t template_array_type[ARRAY_SIZE] = {

    {"table.this",	10,	TABLE_THIS},
    {"table.next",	10,	TABLE_NEXT},
    {"table.rand",	10,	TABLE_RAND},
    {"table.loop",	10,	TABLE_LOOP},
    {"table.link",	10,	TABLE_LINK},

    {"link.this.this",	14,	LINK_THIS_THIS},
    {"link.this.next",	14,	LINK_THIS_NEXT},
    {"link.this.rand",	14,	LINK_THIS_RAND},
    {"link.this.jump",	14,	LINK_THIS_JUMP},
    {"link.this.null",	14,	LINK_THIS_NULL},

    {"link.next.this",	14,	LINK_NEXT_THIS},
    {"link.next.next",	14,	LINK_NEXT_NEXT},
    {"link.next.rand",	14,	LINK_NEXT_RAND},
    {"link.next.jump",	14,	LINK_NEXT_JUMP},
    {"link.next.null",	14,	LINK_NEXT_NULL},

    {"link.rand.this",	14,	LINK_RAND_THIS},
    {"link.rand.next",	14,	LINK_RAND_NEXT},
    {"link.rand.rand",	14,	LINK_RAND_RAND},
    {"link.rand.jump",	14,	LINK_RAND_JUMP},
    {"link.rand.null",	14,	LINK_RAND_NULL},

    {"link.jump.this",	14,	LINK_JUMP_THIS},
    {"link.jump.next",	14,	LINK_JUMP_NEXT},
    {"link.jump.rand",	14,	LINK_JUMP_RAND},
    {"link.jump.jump",	14,	LINK_JUMP_JUMP},
    {"link.jump.null",	14,	LINK_JUMP_NULL},

    {"link.copy",	9,	LINK_COPY},

    {"search-engine",	13,	SEARCH_ENGINE},
};

static char *lg_buf_copy(char *buf)
{
    if (!buf) return NULL;

    int len = strlen(buf);
    if (!len) return NULL;

    char *_buf = je_malloc(len + 1);
    if (!_buf) return NULL;

    memcpy(_buf, buf, len);
    _buf[len] = 0;
    return _buf;
}

void web_site_template_element_create(template_t *template, int type, char *html, char *table_name, char *col_name)
{
    template_node_t *node = je_malloc(sizeof(template_node_t));
    if (!node) return;

    node->type			= type;
    node->html			= lg_buf_copy(html);
    node->table_name		= lg_buf_copy(table_name);
    node->col_name		= lg_buf_copy(col_name);

    lg_list_add(&template->head, &node->next);
}

template_type_t *web_site_template_element_cmp(template_t *template, char *buf)
{
    int i;
    for (i = 0; i < ARRAY_SIZE; i++) {
	if (strncmp(buf, template_array_type[i].cmp, template_array_type[i].len) == 0) {
	    template->node_type_count[template_array_type[i].type]++;
	    return &template_array_type[i];
	}
    }
    return NULL;
}

void web_site_template_element_parse(template_t *template, char *buf)
{
    int len = strlen(buf);
    if (!len) return;

    template_type_t *node_type = web_site_template_element_cmp(template, buf);
    if (!node_type) return;

    char *save = NULL;
    char *table_name = strtok_r(buf + node_type->len, ".\r\n", &save);
    char *col_name = strtok_r(NULL, ".\r\n", &save);
    return web_site_template_element_create(template, node_type->type, NULL, table_name, col_name);
}

void web_site_template_parse(template_t *template, char *buf)
{
    char element[1048576];
    char *pos_element = element;

    char ch = 0;
    char *str = NULL;
    for (str = buf; *str; str++) {

	char ch_pre = ch;
	ch = *str;

	if (ch_pre == '<' && ch == '#') {

	    *(pos_element - 1) = 0;
	    web_site_template_element_create(template, HTML, element, NULL, NULL);
	    pos_element = element;

	} else if (ch_pre == '#' && ch == '>') {

	    *(pos_element - 1) = 0;
	    web_site_template_element_parse(template, element);
	    pos_element = element;

	} else {
	    *pos_element++ = *str;
	}
    }

    *pos_element = 0;
    web_site_template_element_create(template, HTML, element, NULL, NULL);
}

void web_site_template_display(template_t *template)
{

    int i;
    lg_list_t *p = NULL;
    lg_list_for_each(p, &template->head) {
	template_node_t *node = (template_node_t *)p;
	for (i = 0; i < ARRAY_SIZE; i++) {
	    if (node->type == template_array_type[i].type) {
		fprintf(stdout, "[%d][%s][%s][%s]\n", 
			template_array_type[i].type, 
			template_array_type[i].cmp,
			node->table_name,
			node->col_name);
	    }
	}
    }

    for (i = 0; i < ARRAY_SIZE; i++) {
	if (template->node_type_count[template_array_type[i].type]) {
	    fprintf(stdout, "[%d][%s][%d]\n", 
		    template_array_type[i].type, 
		    template_array_type[i].cmp,
		    template->node_type_count[template_array_type[i].type]
		   );
	}
    }

    fprintf(stdout, "=================\n");
}

template_t *web_site_template_create(char *buf)
{
    if (!buf || !strlen(buf))
	return NULL;

    template_t *template = je_malloc(sizeof(template_t));
    if (!template) return NULL;

    memset(template, 0, sizeof(template_t));
    template->buf = buf;

    lg_list_init(&template->head);
    web_site_template_parse(template, buf);
    // web_site_template_display(template);
    return template;
}



