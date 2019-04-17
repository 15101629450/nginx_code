
#ifndef __LG_DB_H__
#define __LG_DB_H__

#include "lg_list.h"
#include "lg_tree.h"

#ifdef __JEMALLOC__
#include <jemalloc/jemalloc.h>
#else
#include <stdlib.h>
#define je_malloc malloc
#define je_free	free
#endif

#define LG_DB_TYPE_STRING	1
#define LG_DB_TYPE_NUMBER	2
#define LG_DB_TYPE_NULL		3
#define LG_DB_TYPE_ERROR	4

typedef struct {

    int type;			// 1:str 2:int 3:null
    union {
	char *string;
	int number;
    };

} lg_db_data_t;

typedef struct __lg_db_t lg_db_t;
struct __lg_db_t {

    lg_db_data_t	key;
    lg_db_data_t	value;
    void		*m_struct;

    lg_db_t		*parent;		// 父节点
    int			child_total;		// 子节点数量

    lg_list_t 		child_list;		// 子节点根 -> next_list
    lg_tree_head 	child_tree_string;	// 子节点根 -> next_tree_string
    lg_tree_head 	child_tree_number;	// 子节点根 -> next_tree_number

    lg_list_t		next_list;		// 节点 list 		<- 父节点 child_list
    lg_tree_node	next_tree_string;	// 节点 tree string	<- 父节点 child_tree_string
    lg_tree_node	next_tree_number;	// 节点 tree number	<- 父节点 child_tree_number
};

/*** data type ***/
lg_db_data_t lg_db_data_string(char *key);
lg_db_data_t lg_db_data_number(int key);
lg_db_data_t lg_db_data_null(void);

/*** db create/free ***/
lg_db_t *lg_db_create(void);
void lg_db_free(lg_db_t *db);

/*** db find ***/
lg_db_t *lg_db_find(lg_db_t *db, lg_db_data_t key);
lg_db_t *lg_db_find_args(lg_db_t *db, ...);

/*** db add ***/
lg_db_t *lg_db_add(lg_db_t *db, lg_db_data_t key, lg_db_data_t value);

/*** db display ***/
void lg_db_data_display(lg_db_data_t data);
void lg_db_display(lg_db_t *db);

/*** db to type ***/
char *lg_db_to_string(lg_db_t *db);
int lg_db_to_number(lg_db_t *db);
void *lg_db_to_struct(lg_db_t *db);

#endif



