
#ifndef	__LG_RBTREE_H__
#define	__LG_RBTREE_H__

#include <stdio.h>
#include <string.h>

typedef struct lg_tree_node {

    unsigned long  rb_parent_color;
    struct lg_tree_node *rb_right;
    struct lg_tree_node *rb_left;

    union {
	int number;
	char *string;
    };

} lg_tree_node ;

typedef struct {

    lg_tree_node *rb_node;

} lg_tree_head;

extern void lg_tree_insert_color(lg_tree_node *node, lg_tree_head *head);
extern void lg_tree_link_node(lg_tree_node * node, lg_tree_node *parent, lg_tree_node **rb_link);
extern void lg_tree_delete(lg_tree_head *head, lg_tree_node *node);

/* string */ 
extern lg_tree_node *lg_tree_string_add(lg_tree_head *head, lg_tree_node *new_node);
extern lg_tree_node *lg_tree_string_find(lg_tree_head *head, const char *key);

/* number */
extern lg_tree_node *lg_tree_number_add(lg_tree_head *head, lg_tree_node *new_node);
extern lg_tree_node *lg_tree_number_find(lg_tree_head *head, int key);

#define RB_ROOT	(lg_tree_head) {0}
#define rb_is_red(r) (!rb_color(r))
#define rb_is_black(r) rb_color(r)
#define rb_set_red(r) do { (r)->rb_parent_color &= ~1; } while (0)
#define rb_set_black(r) do { (r)->rb_parent_color |= 1; } while (0)
#define rb_parent(r) ((lg_tree_node *)((r)->rb_parent_color & ~3))
#define rb_color(r) ((r)->rb_parent_color & 1)
#define rb_offsetof(type,member) (size_t)(&((type *)0)->member)
#define rb_entry(ptr, type, member) ({const typeof(((type *)0)->member) * __mptr = (ptr);(type *)((char *)__mptr - rb_offsetof(type, member));})

#endif


