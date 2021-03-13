/* SPDX-License-Identifier: MIT */
/* Minimal Splay-tree helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#ifndef __SPLAYTREE_COMMON_TREEPRINT_H__
#define __SPLAYTREE_COMMON_TREEPRINT_H__

#include <stdio.h>

#include "../splaytree.h"
#include "common.h"

static __inline__ void printnode(const struct splay_node *node, size_t depth,
				 char prefix)
{
	const struct splayitem *item;
	size_t i;

	if (!node)
		return;

	item = splay_entry(node, struct splayitem, splay);

	printnode(node->right, depth+1, '/');

	for (i = 0; i < depth; i++)
		printf("    ");

	printf("%c%03u\n", prefix, item->i);

	printnode(node->left, depth+1, '\\');

}

static __inline__ void printtree(const struct splay_root *root)
{
	printnode(root->node, 0, '*');
}

static __inline__ void printnode_dot(const struct splay_node *node,
				     size_t *nilcnt)
{
	const struct splayitem *item;
	const struct splayitem *citem;

	if (!node)
		return;

	item = splay_entry(node, struct splayitem, splay);
	printf("%03u;\n", item->i);

	if (node->left) {
		citem = splay_entry(node->left, struct splayitem, splay);
		printf("%03u:sw -> %03u;\n", item->i, citem->i);
	} else {
		printf("nil%zu [label=\"NIL\", shape=box, color=\"black\"];\n",
		       *nilcnt);
		printf("%03u:sw -> nil%zu [color=\"black\"];\n", item->i,
		       *nilcnt);

		(*nilcnt)++;
	}

	if (node->right) {
		citem = splay_entry(node->right, struct splayitem, splay);
		printf("%03u:se -> %03u;\n", item->i, citem->i);
	} else {
		printf("nil%zu [label=\"NIL\", shape=box, color=\"black\"];\n",
		       *nilcnt);
		printf("%03u:se -> nil%zu [color=\"black\"];\n", item->i,
		       *nilcnt);

		(*nilcnt)++;
	}

	printnode_dot(node->left, nilcnt);
	printnode_dot(node->right, nilcnt);
}

static __inline__ void printtree_dot(const struct splay_root *root)
{
	size_t nilcnt = 0;

	printf("digraph G {\n");
	printf("  graph [ordering=\"out\"];\n");

	printnode_dot(root->node, &nilcnt);

	printf("}\n");
}

#endif /* __SPLAYTREE_COMMON_TREEPRINT_H__ */
