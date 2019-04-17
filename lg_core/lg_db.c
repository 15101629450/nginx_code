
#include "lg_db.h"
#include <stdarg.h>

/**************/
/* lg_db_data */
/**************/

// data string
lg_db_data_t lg_db_data_string(char *key)
{
    lg_db_data_t data;
    data.type = key ? LG_DB_TYPE_STRING : LG_DB_TYPE_ERROR;
    data.string = key;
    return data;
}

// data number
lg_db_data_t lg_db_data_number(int key)
{
    lg_db_data_t data;
    data.type = LG_DB_TYPE_NUMBER;
    data.number = key;
    return data;
}

// data null
lg_db_data_t lg_db_data_null(void)
{
    lg_db_data_t data;
    data.type = LG_DB_TYPE_NULL;
    data.number = 0;
    return data;
}

// data free
void lg_db_data_free(lg_db_data_t data)
{
    if (data.type == LG_DB_TYPE_STRING && data.string) {
	je_free(data.string);
    }
}

// buf copy
char *lg_db_buf_copy(char *src)
{
    if (!src) return NULL;

    int len = strlen(src);
    char *buf = je_malloc(len + 1);
    if (!buf) return NULL;

    memcpy(buf, src, len);
    buf[len] = 0;
    return buf;
}

/**************/
/*   lg_db    */
/**************/

// db create
lg_db_t *lg_db_create(void)
{
    lg_db_t *db = je_malloc(sizeof(lg_db_t));
    if (!db) return NULL;
    memset(db, 0, sizeof(lg_db_t));

    // 初始化下级节点
    lg_list_init(&db->child_list);
    db->child_tree_string = RB_ROOT;
    db->child_tree_number = RB_ROOT;
    return db;
}

// db free
void lg_db_free(lg_db_t *db)
{
    lg_list_t *p = NULL;
    lg_list_t *head = &db->child_list;
    for (p = head->next; p != head; ) {

	lg_db_t *node = lg_list_entry(p, lg_db_t, next_list);
	p = p->next;

	lg_db_free(node);	// 递归子节点
    }

    lg_db_data_free(db->key);	// 释放 key
    lg_db_data_free(db->value);	// 释放 value
    je_free(db);		// 释放 db
}

static void *__lg_db_free(lg_db_t *db)
{
    lg_db_free(db);
    return NULL;
}

// db find
lg_db_t *lg_db_find(lg_db_t *db, lg_db_data_t key)
{
    if (!db) return NULL;
    if (key.type == LG_DB_TYPE_STRING) {

	lg_tree_node *node = lg_tree_string_find(&db->child_tree_string, key.string);
	if (node) return rb_entry(node, lg_db_t, next_tree_string);

    } else if (key.type == LG_DB_TYPE_NUMBER) {

	lg_tree_node *node = lg_tree_number_find(&db->child_tree_number, key.number);
	if (node) return rb_entry(node, lg_db_t, next_tree_number);
    }

    return NULL;
}

// db find args
lg_db_t *lg_db_find_args(lg_db_t *db, ...)
{
    va_list args;
    va_start(args, db);

    do {

	lg_db_data_t key = va_arg(args, lg_db_data_t);
	switch(key.type) {
	    case LG_DB_TYPE_STRING :
	    case LG_DB_TYPE_NUMBER :
		db = lg_db_find(db, key);
		break;

	    case LG_DB_TYPE_NULL :
		va_end(args);
		return db;

	    default :
		va_end(args);
		return NULL;
	}

    } while(db);

    va_end(args);
    return NULL;
}

// db add
lg_db_t *lg_db_add(lg_db_t *db, lg_db_data_t key, lg_db_data_t value)
{
    if (!db) return NULL;
    lg_db_t *node = lg_db_find(db, key);
    if (node) return node;

    node = lg_db_create();
    if (!node) return NULL;

    switch(key.type) {

	case LG_DB_TYPE_STRING: // add string
	    node->key.type = LG_DB_TYPE_STRING;
	    node->key.string = lg_db_buf_copy(key.string);
	    if (!node->key.string) return __lg_db_free(node);
	    node->next_tree_string.string = node->key.string;
	    break;

	case LG_DB_TYPE_NUMBER: // add number
	    node->key.type = LG_DB_TYPE_NUMBER;
	    node->key.number = key.number;
	    node->next_tree_number.number = node->key.number;
	    break;

	default: return __lg_db_free(node);
    }

    switch(value.type) {

	case LG_DB_TYPE_STRING: // add string
	    node->value.type = LG_DB_TYPE_STRING;
	    node->value.string = lg_db_buf_copy(value.string);
	    if (!node->value.string) return __lg_db_free(node);
	    break;

	case LG_DB_TYPE_NUMBER: // add number
	    node->value.type = LG_DB_TYPE_NUMBER;
	    node->value.number = value.number;
	    break;

	case LG_DB_TYPE_NULL: // add null
	    node->value.type = LG_DB_TYPE_NULL;
	    node->value.number = 0;
	    break;

	default: return __lg_db_free(node);
    }

    node->parent = db;
    db->child_total++;
    lg_list_add(&db->child_list, &node->next_list);

    if (node->key.type == LG_DB_TYPE_STRING)
	lg_tree_string_add(&db->child_tree_string, &node->next_tree_string);
    else if (node->key.type == LG_DB_TYPE_NUMBER)
	lg_tree_number_add(&db->child_tree_number, &node->next_tree_number);

    return node;
}

// data display
void lg_db_data_display(lg_db_data_t data)
{
    switch(data.type) {
	case LG_DB_TYPE_STRING:
	    fprintf(stdout, "[%s]", data.string);
	    break;

	case LG_DB_TYPE_NUMBER:
	    fprintf(stdout, "[%d]", data.number);
	    break;

	case LG_DB_TYPE_NULL:
	    fprintf(stdout, "[null]");
	    break;
    }
}

// db display
void lg_db_display(lg_db_t *db)
{
    lg_list_t *p = NULL;
    lg_list_for_each(p, &db->child_list) {

	lg_db_t *node = lg_list_entry(p, lg_db_t, next_list);
	lg_db_t *parent = node->parent;
	while(parent && parent->parent) {
	    fprintf(stdout, "│    ");
	    parent = parent->parent;
	}

	fprintf(stdout, "├──");
	lg_db_data_display(node->key);
	lg_db_data_display(node->value);
	fprintf(stdout, "[%d]\n", node->child_total);

	lg_db_display(node);
    }
}

// db to string
char *lg_db_to_string(lg_db_t *db)
{
    if (!db || db->value.type != LG_DB_TYPE_STRING)
	return NULL;
    return db->value.string;
}

// db to number
int lg_db_to_number(lg_db_t *db)
{
    if (!db || db->value.type != LG_DB_TYPE_NUMBER)
	return 0;
    return db->value.number;
}

// db to struct
void *lg_db_to_struct(lg_db_t *db)
{
    if (!db) return NULL;
    return db->m_struct;
}


