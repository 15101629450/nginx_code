
#include "lg_tree.h"

void lg_tree_set_parent(lg_tree_node *rb, lg_tree_node *p)
{
    rb->rb_parent_color = (rb->rb_parent_color & 3) | (unsigned long)p;
}

void lg_tree_set_color(lg_tree_node *rb, int color)
{
    rb->rb_parent_color = (rb->rb_parent_color & ~1) | color;
}

void lg_tree_link_node(lg_tree_node *node, lg_tree_node *parent, lg_tree_node **rb_link)
{

    node->rb_parent_color = (unsigned long )parent;
    node->rb_left = node->rb_right = NULL;
    *rb_link = node;
}

void lg_tree_rotate_left(lg_tree_node *node, lg_tree_head *head)
{
    lg_tree_node *right = node->rb_right;
    lg_tree_node *parent = rb_parent(node);

    if ((node->rb_right = right->rb_left))
	lg_tree_set_parent(right->rb_left, node);

    right->rb_left = node;
    lg_tree_set_parent(right, parent);

    if (parent) {

	if (node == parent->rb_left)
	    parent->rb_left = right;
	else
	    parent->rb_right = right;

    } else {

	head->rb_node = right;
    }

    lg_tree_set_parent(node, right);
}

void lg_tree_rotate_right(lg_tree_node *node, lg_tree_head *head)
{
    lg_tree_node *left = node->rb_left;
    lg_tree_node *parent = rb_parent(node);

    if ((node->rb_left = left->rb_right))
	lg_tree_set_parent(left->rb_right, node);

    left->rb_right = node;
    lg_tree_set_parent(left, parent);

    if (parent) {

	if (node == parent->rb_right)
	    parent->rb_right = left;
	else
	    parent->rb_left = left;

    } else {

	head->rb_node = left;
    }

    lg_tree_set_parent(node, left);
}

void lg_tree_insert_color(lg_tree_node *node, lg_tree_head *head)
{

    lg_tree_node *parent, *gparent;

    while ((parent = rb_parent(node)) && rb_is_red(parent)) {

	gparent = rb_parent(parent);

	if (parent == gparent->rb_left) {

	    register lg_tree_node *uncle = gparent->rb_right;
	    if (uncle && rb_is_red(uncle)) {

		rb_set_black(uncle);
		rb_set_black(parent);
		rb_set_red(gparent);
		node = gparent;
		continue;
	    }

	    if (parent->rb_right == node) {

		register lg_tree_node *tmp;
		lg_tree_rotate_left(parent, head);
		tmp = parent;
		parent = node;
		node = tmp;
	    }

	    rb_set_black(parent);
	    rb_set_red(gparent);
	    lg_tree_rotate_right(gparent, head);

	} else {

	    register lg_tree_node *uncle = gparent->rb_left;
	    if (uncle && rb_is_red(uncle)) {

		rb_set_black(uncle);
		rb_set_black(parent);
		rb_set_red(gparent);
		node = gparent;
		continue;
	    }

	    if (parent->rb_left == node) {

		register lg_tree_node *tmp;
		lg_tree_rotate_right(parent, head);
		tmp = parent;
		parent = node;
		node = tmp;
	    }

	    rb_set_black(parent);
	    rb_set_red(gparent);
	    lg_tree_rotate_left(gparent, head);
	}
    }

    rb_set_black(head->rb_node);
}

void lg_tree_delete_color(lg_tree_node *node, lg_tree_node *parent, lg_tree_head *head)
{

    lg_tree_node *other = NULL;

    while ((!node || rb_is_black(node)) && node != head->rb_node) {

	if (parent->rb_left == node) {

	    other = parent->rb_right;

	    if (rb_is_red(other)) {

		rb_set_black(other);
		rb_set_red(parent);
		lg_tree_rotate_left(parent, head);
		other = parent->rb_right;
	    }

	    if ((!other->rb_left || rb_is_black(other->rb_left)) &&
		    (!other->rb_right || rb_is_black(other->rb_right))) {

		rb_set_red(other);
		node = parent;
		parent = rb_parent(node);

	    } else {

		if (!other->rb_right || rb_is_black(other->rb_right)) {

		    rb_set_black(other->rb_left);
		    rb_set_red(other);
		    lg_tree_rotate_right(other, head);
		    other = parent->rb_right;
		}

		lg_tree_set_color(other, rb_color(parent));
		rb_set_black(parent);
		rb_set_black(other->rb_right);
		lg_tree_rotate_left(parent, head);
		node = head->rb_node;
		break;
	    }

	} else {

	    other = parent->rb_left;

	    if (rb_is_red(other)) {

		rb_set_black(other);
		rb_set_red(parent);
		lg_tree_rotate_right(parent, head);
		other = parent->rb_left;
	    }

	    if ((!other->rb_left || rb_is_black(other->rb_left)) &&
		    (!other->rb_right || rb_is_black(other->rb_right))) {

		rb_set_red(other);
		node = parent;
		parent = rb_parent(node);

	    } else {

		if (!other->rb_left || rb_is_black(other->rb_left)) {

		    rb_set_black(other->rb_right);
		    rb_set_red(other);
		    lg_tree_rotate_left(other, head);
		    other = parent->rb_left;
		}

		lg_tree_set_color(other, rb_color(parent));
		rb_set_black(parent);
		rb_set_black(other->rb_left);
		lg_tree_rotate_right(parent, head);
		node = head->rb_node;
		break;
	    }
	}
    }

    if (node)rb_set_black(node);
}

void lg_tree_delete(lg_tree_head *head, lg_tree_node *node)
{

    int color;
    lg_tree_node *child, *parent;

    if (!node->rb_left) {

	child = node->rb_right;

    } else if (!node->rb_right) {

	child = node->rb_left;

    } else {

	lg_tree_node *old = node, *left;

	node = node->rb_right;
	while ((left = node->rb_left) != NULL)
	    node = left;

	if (rb_parent(old)) {

	    if (rb_parent(old)->rb_left == old)
		rb_parent(old)->rb_left = node;
	    else
		rb_parent(old)->rb_right = node;

	} else {

	    head->rb_node = node;
	}

	child = node->rb_right;
	parent = rb_parent(node);
	color = rb_color(node);

	if (parent == old) {

	    parent = node;

	} else {

	    if (child)lg_tree_set_parent(child, parent);
	    parent->rb_left = child;

	    node->rb_right = old->rb_right;
	    lg_tree_set_parent(old->rb_right, node);
	}

	node->rb_parent_color = old->rb_parent_color;
	node->rb_left = old->rb_left;
	lg_tree_set_parent(old->rb_left, node);

	if (color == 1)lg_tree_delete_color(child, parent, head);
	return;
    }

    parent = rb_parent(node);
    color = rb_color(node);

    if (child)lg_tree_set_parent(child, parent);

    if (parent) {

	if (parent->rb_left == node)
	    parent->rb_left = child;
	else
	    parent->rb_right = child;

    } else {

	head->rb_node = child;
    }

    if (color == 1)lg_tree_delete_color(child, parent, head);
}

/* string */
lg_tree_node *lg_tree_string_add(lg_tree_head *head, lg_tree_node *new_node)
{
    lg_tree_node *parent = NULL;
    lg_tree_node **p = &head->rb_node;
    lg_tree_node *node = NULL;

    if (new_node->string == NULL)
	return NULL;

    while (*p) {

	parent = *p;
	node = (lg_tree_node *)parent;
	if (node->string == NULL)
	    return NULL;

	int retval = strcmp(new_node->string, node->string);
	if (retval < 0) {

	    p = &(*p)->rb_left;

	} else if (retval > 0) {

	    p = &(*p)->rb_right;

	} else {

	    return NULL;
	}
    }

    lg_tree_link_node(new_node, parent, p);
    lg_tree_insert_color(new_node, head);
    return node;
}

lg_tree_node *lg_tree_string_find(lg_tree_head *head, const char *key)
{
    lg_tree_node *p = head->rb_node;
    lg_tree_node *node = NULL;

    if (key == NULL)
	return NULL;

    while (p) {

	node = (lg_tree_node *)p;
	if (node->string == NULL)
	    return NULL;

	int retval = strcmp(key, node->string);
	if (retval < 0) {

	    p = p->rb_left;

	} else if (retval > 0) {

	    p = p->rb_right;

	} else {

	    return node;
	}
    }

    return NULL;
}

/* number */
lg_tree_node *lg_tree_number_add(lg_tree_head *head, lg_tree_node *new_node)
{

    lg_tree_node *parent = NULL;
    lg_tree_node **p = &head->rb_node;
    lg_tree_node *node = NULL;

    while (*p) {

	parent = *p;
	node = (lg_tree_node *)parent;

	if (new_node->number < node->number) {

	    p = &(*p)->rb_left;

	} else if (new_node->number > node->number) {

	    p = &(*p)->rb_right;

	} else {

	    return NULL;
	}
    }

    lg_tree_link_node(new_node, parent, p);
    lg_tree_insert_color(new_node, head);
    return node;
}

lg_tree_node *lg_tree_number_find(lg_tree_head *head, int key)
{

    lg_tree_node *p = head->rb_node;
    lg_tree_node *node = NULL;

    while (p) {

	node = (lg_tree_node *)p;

	if (key < node->number) {

	    p = p->rb_left;

	} else if (key > node->number) {

	    p = p->rb_right;

	} else {

	    return node;
	}
    }

    return NULL;
}

/* print */
void _lg_tree_print(lg_tree_node *p)
{
    // lg_tree_node *node = (lg_tree_node *)p;
    // fprintf(stdout, "[tree][%d][%s]\n", node->number, node->string);
    if (p->rb_left) _lg_tree_print(p->rb_left);
    if (p->rb_right)_lg_tree_print(p->rb_right);
}

void lg_tree_print(lg_tree_head *head)
{
    if (!head->rb_node) return;
    _lg_tree_print(head->rb_node);
}


