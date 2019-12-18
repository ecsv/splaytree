// SPDX-License-Identifier: MIT
/* Minimal Splay-tree helper functions
 *
 * SPDX-FileCopyrightText: 2012-2019, Sven Eckelmann <sven@narfation.org>
 */

#include "splaytree.h"

#include <stdbool.h>
#include <stddef.h>

/**
 * splay_change_child() - Fix child entry of parent node
 * @old_node: splay node to replace
 * @new_node: splay node replacing @old_node
 * @parent: parent of @old_node
 * @root: pointer to splay root
 *
 * Detects if @old_node is left/right child of @parent or if it gets inserted
 * as as new root. These entries are then updated to point to @new_node.
 *
 * @old_node and @root must not be NULL.
 */
static void splay_change_child(struct splay_node *old_node,
			       struct splay_node *new_node,
			       struct splay_node *parent,
			       struct splay_root *root)
{
	if (parent) {
		if (parent->left == old_node)
			parent->left = new_node;
		else
			parent->right = new_node;
	} else {
		root->node = new_node;
	}
}

/**
 * splay_rotate_switch_parents() - set parent for switched nodes after rotate
 * @node_top: splay node which became the new top node
 * @node_child: splay node which became the new child node
 * @node_child2: ex'child of @node_top which now is now 2. child of @node_child
 * @root: pointer to splay root
 *
 * @node_top must have been a valid child of @node_child. The child changes
 * for the rotation in @node_child and @node_top must already be finished.
 * The switch of parents for @node_top, @node_child and @node_child2
 * (when it exists) is peformend. The change of the child entry of the new
 * parent of @node_top is done afterwards.
 */
static void splay_rotate_switch_parents(struct splay_node *node_top,
					struct splay_node *node_child,
					struct splay_node *node_child2,
					struct splay_root *root)
{
	/* switch parents and set new balance */
	node_top->parent = node_child->parent;
	node_child->parent = node_top;

	/* switch parent of child2 from child to top */
	if (node_child2)
		node_child2->parent = node_child;

	/* parent of node_top must get its child pointer get fixed */
	splay_change_child(node_child, node_top, node_top->parent, root);
}

/**
 * splay_is_right_child() - Check if the node is a right child
 * @node: splay node to check
 *
 * Return: true when @node is a right child, false when it is a left child or
 *  when it has no parent
 */
static bool splay_is_right_child(struct splay_node *node)
{
	struct splay_node *parent = node->parent;

	if (!parent)
		return false;

	if (parent->right == node)
		return true;

	return false;
}

/**
 * splay_rotate_left() - Rotate subtree at @parent to the left
 * @node: right node of @parent which moves balance to the right
 * @parent: root of the subtree to rotate to the left
 * @root: pointer to splay root
 */
static struct splay_node *splay_rotate_left(struct splay_node *parent,
					    struct splay_root *root)
{
	struct splay_node *tmp;

	/* rotate left */
	tmp = parent->right;
	parent->right = tmp->left;
	tmp->left = parent;

	splay_rotate_switch_parents(tmp, parent, parent->right, root);

	return tmp;
}

/**
 * splay_rotate_right() - Rotate subtree at @parent to the right
 * @node: left node of @parent which moves balance to the left
 * @parent: root of the subtree to rotate to the right
 * @root: pointer to splay root
 */
static struct splay_node *splay_rotate_right(struct splay_node *parent,
					     struct splay_root *root)
{
	struct splay_node *tmp;

	/* rotate right */
	tmp = parent->left;
	parent->left = tmp->right;
	tmp->right = parent;

	splay_rotate_switch_parents(tmp, parent, parent->left, root);

	return tmp;
}

/**
 * splay_splaying() - Go tree upwards and splay @node to the root
 * @node: pointer to the new node
 * @root: pointer to splay root
 *
 * The tree is traversed from bottom to the top starting at @node. The @node
 * will be moved upwards towards the @root of the tree.
 */
void splay_splaying(struct splay_node *node, struct splay_root *root)
{
	struct splay_node *parent;

	while (node->parent) {
		parent = node->parent;
		if (!parent->parent) {
			/* zig step */
			if (splay_is_right_child(node))
				splay_rotate_left(parent, root);
			else
				splay_rotate_right(parent, root);
		} else {
			if (splay_is_right_child(node)) {
				if (splay_is_right_child(parent)) {
					/* zig-zig step */
					splay_rotate_left(parent->parent, root);
					splay_rotate_left(parent, root);
				} else {
					/* zig-zag step */
					splay_rotate_left(parent, root);
					splay_rotate_right(node->parent, root);
				}
			} else {
				if (splay_is_right_child(parent)) {
					/* zig-zag step */
					splay_rotate_right(parent, root);
					splay_rotate_left(node->parent, root);
				} else {
					/* zig-zig step */
					splay_rotate_right(parent->parent,
							   root);
					splay_rotate_right(parent, root);
				}
			}
		}
	}
}

/**
 * splay_erase_node() - Remove splay node from tree
 * @node: pointer to the node
 * @root: pointer to splay root
 * @removed_right: returns whether returned node now has a decreased depth under
 *  the right child
 *
 * The node is only removed from the tree. Neither the memory of the removed
 * node nor the memory of the entry containing the node is free'd. The node
 * has to be handled like an uninitialized node. Accessing the parent or
 * right/left pointer of the node is not safe.
 *
 * WARNING A call to splay_splaying after splay_erase_node is required to follow
 * the standard definition of a splay tree. splay_erase can be used as helper to
 * run both steps at the same time.
 *
 * Return: parent of the removed node, NULL if no parent is available
 */
struct splay_node *splay_erase_node(struct splay_node *node,
				    struct splay_root *root)
{
	struct splay_node *smallest;
	struct splay_node *smallest_parent;
	struct splay_node *decreased_node;

	if (!node->left && !node->right) {
		/* no child
		 * just delete the current child
		 */
		splay_change_child(node, NULL, node->parent, root);

		return node->parent;
	} else if (node->left && !node->right) {
		/* one child, left
		 * use left child as replacement for the deleted node
		 */
		node->left->parent = node->parent;
		splay_change_child(node, node->left, node->parent, root);

		return node->parent;
	} else if (!node->left) {
		/* one child, right
		 * use right child as replacement for the deleted node
		 */
		node->right->parent = node->parent;
		splay_change_child(node, node->right, node->parent, root);

		return node->parent;
	}

	/* two children, take smallest of right (grand)children */
	smallest = node->right;
	while (smallest->left)
		smallest = smallest->left;

	smallest_parent = smallest->parent;
	if (smallest == node->right)
		decreased_node = node->right;
	else
		decreased_node = smallest_parent;

	/* move right child of smallest one up */
	if (smallest->right)
		smallest->right->parent = smallest_parent;
	splay_change_child(smallest, smallest->right, smallest_parent, root);

	/* exchange node with smallest */
	smallest->parent = node->parent;

	smallest->left = node->left;
	smallest->left->parent = smallest;

	smallest->right = node->right;
	if (smallest->right)
		smallest->right->parent = smallest;

	splay_change_child(node, smallest, node->parent, root);

	return decreased_node;
}

/**
 * splay_first() - Find leftmost splay node in tree
 * @root: pointer to splay root
 *
 * Return: pointer to leftmost node. NULL when @root is empty.
 */
struct splay_node *splay_first(const struct splay_root *root)
{
	struct splay_node *node = root->node;

	if (!node)
		return node;

	/* descend down via smaller/preceding child */
	while (node->left)
		node = node->left;

	return node;
}

/**
 * splay_last() - Find rightmost splay node in tree
 * @root: pointer to splay root
 *
 * Return: pointer to rightmost node. NULL when @root is empty.
 */
struct splay_node *splay_last(const struct splay_root *root)
{
	struct splay_node *node = root->node;

	if (!node)
		return node;

	/* descend down via larger/succeeding child */
	while (node->right)
		node = node->right;

	return node;
}

/**
 * splay_next() - Find successor node in tree
 * @node: starting splay node for search
 *
 * Return: pointer to successor node. NULL when no successor of @node exist.
 */
struct splay_node *splay_next(struct splay_node *node)
{
	struct splay_node *parent;

	/* there is a right child - next node must be the leftmost under it */
	if (node->right) {
		node = node->right;
		while (node->left)
			node = node->left;

		return node;
	}

	/* otherwise check if we have a parent (and thus maybe siblings) */
	parent = node->parent;
	if (!parent)
		return parent;

	/* go up the tree until the path connecting both is the left child
	 * pointer and therefore the parent is the next node
	 */
	while (parent && parent->right == node) {
		node = parent;
		parent = node->parent;
	}

	return parent;
}

/**
 * splay_prev() - Find predecessor node in tree
 * @node: starting splay node for search
 *
 * Return: pointer to predecessor node. NULL when no predecessor of @node exist.
 */
struct splay_node *splay_prev(struct splay_node *node)
{
	struct splay_node *parent;

	/* there is a left child - prev node must be the rightmost under it */
	if (node->left) {
		node = node->left;
		while (node->right)
			node = node->right;

		return node;
	}

	/* otherwise check if we have a parent (and thus maybe siblings) */
	parent = node->parent;
	if (!parent)
		return parent;

	/* go up the tree until the path connecting both is the right child
	 * pointer and therefore the parent is the prev node
	 */
	while (parent && parent->left == node) {
		node = parent;
		parent = node->parent;
	}

	return parent;
}
