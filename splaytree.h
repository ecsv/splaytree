/* SPDX-License-Identifier: MIT */
/* Minimal Splay-tree helper functions
 *
 * SPDX-FileCopyrightText: 2012-2019, Sven Eckelmann <sven@narfation.org>
 */

#ifndef __SPLAYTREE_H__
#define __SPLAYTREE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#if defined(__GNUC__)
#define SPLAYTREE_TYPEOF_USE 1
#endif

#if defined(_MSC_VER)
#define __inline__ __inline
#endif

/**
 * container_of() - Calculate address of object that contains address ptr
 * @ptr: pointer to member variable
 * @type: type of the structure containing ptr
 * @member: name of the member variable in struct @type
 *
 * Return: @type pointer of object containing ptr
 */
#ifndef container_of
#ifdef SPLAYTREE_TYPEOF_USE
#define container_of(ptr, type, member) __extension__ ({ \
	const __typeof__(((type *)0)->member) *__pmember = (ptr); \
	(type *)((char *)__pmember - offsetof(type, member)); })
#else
#define container_of(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))
#endif
#endif

/**
 * struct splay_node - node of an splay tree
 * @parent: pointer to the parent node in the tree
 * @left: pointer to the left child in the tree
 * @right: pointer to the right child in the tree
 *
 * The splay tree consists of a root and nodes attached to this root. The
 * splay_* functions and macros can be used to access and modify this data
 * structure.
 *
 * The @parent pointer of the splay node points to the parent splay node in the
 * tree, the @left to the left "smaller key" child and @right to the right
 * "larger key" node of the tree.
 *
 * The splay nodes are usually embedded in a container structure which holds the
 * actual data. Such an container object is called entry. The helper splay_entry
 * can be used to calculate the object address from the address of the node.
 */
struct splay_node {
	struct splay_node *parent;
	struct splay_node *left;
	struct splay_node *right;
};

/**
 * struct splay_root - root of an splay-tree
 * @node: pointer to the root node in the tree
 *
 * For an empty tree, node points to NULL.
 */
struct splay_root {
	struct splay_node *node;
};

/**
 * DEFINE_SPLAYROOT - define tree root and initialize it
 * @root: name of the new object
 */
#define DEFINE_SPLAYROOT(root) \
	struct splay_root root = { NULL }

/**
 * INIT_SPLAY_ROOT() - Initialize empty tree
 * @root: pointer to splay root
 */
static __inline__ void INIT_SPLAY_ROOT(struct splay_root *root)
{
	root->node = NULL;
}

/**
 * splay_empty() - Check if tree has no nodes attached
 * @root: pointer to the root of the tree
 *
 * Return: 0 - tree is not empty !0 - tree is empty
 */
static __inline__ int splay_empty(const struct splay_root *root)
{
	return !root->node;
}

/**
 * splay_link_node() - Add new node as new leaf
 * @node: pointer to the new node
 * @parent: pointer to the parent node
 * @splay_link: pointer to the left/right pointer of @parent
 *
 * @node will be initialized as leaf node of @parent. It will be linked to the
 * tree via the @splay_link pointer. @parent must be NULL and @splay_link has to
 * point to "node" of splay_root when the tree is empty.
 *
 * WARNING A call to splay_splaying after splay_link_node is required to follow
 * the standard definition of a splay tree. splay_insert can be used as helper to
 * run both steps at the same time.
 */
static __inline__ void splay_link_node(struct splay_node *node,
				       struct splay_node *parent,
				       struct splay_node **splay_link)
{
	node->parent = parent;
	node->left = NULL;
	node->right = NULL;

	*splay_link = node;
}

void splay_splaying(struct splay_node *node, struct splay_root *root);

/**
 * splay_insert() - Add new node as new leaf and reorder tree
 * @node: pointer to the new node
 * @parent: pointer to the parent node
 * @splay_link: pointer to the left/right pointer of @parent
 * @root: pointer to splay root
 */
static __inline__ void splay_insert(struct splay_node *node,
				    struct splay_node *parent,
				    struct splay_node **splay_link,
				    struct splay_root *root)
{
	splay_link_node(node, parent, splay_link);
	splay_splaying(node, root);
}

struct splay_node *splay_erase_node(struct splay_node *node,
				    struct splay_root *root);

/**
 * splay_erase() - Remove splay node from tree and rebalance tree
 * @node: pointer to the node
 * @root: pointer to splay root
 */
static __inline__ void splay_erase(struct splay_node *node,
				   struct splay_root *root)
{
	struct splay_node *parent;

	parent = splay_erase_node(node, root);
	if (parent)
		splay_splaying(parent, root);
}

struct splay_node *splay_first(const struct splay_root *root);
struct splay_node *splay_last(const struct splay_root *root);
struct splay_node *splay_next(struct splay_node *node);
struct splay_node *splay_prev(struct splay_node *node);

/**
 * splay_entry() - Calculate address of entry that contains tree node
 * @node: pointer to tree node
 * @type: type of the entry containing the tree node
 * @member: name of the splay_node member variable in struct @type
 *
 * Return: @type pointer of entry containing node
 */
#define splay_entry(node, type, member) container_of(node, type, member)

#ifdef __cplusplus
}
#endif

#endif /* __SPLAYTREE_H__ */
