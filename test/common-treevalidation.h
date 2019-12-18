/* SPDX-License-Identifier: MIT */
/* Minimal Splay-tree helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2019, Sven Eckelmann <sven@narfation.org>
 */

#ifndef __SPLAYTREE_COMMON_TREEVALIDATION_H__
#define __SPLAYTREE_COMMON_TREEVALIDATION_H__

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "../splaytree.h"
#include "common.h"

static __inline__ void check_node_order(struct splay_node *node,
					struct splay_node *parent,
					const uint8_t *skiplist, uint16_t *pos,
					uint16_t size)
{
	struct splayitem *item;

	if (!node)
		return;

	assert(node->parent == parent);

	check_node_order(node->left, node, skiplist, pos, size);

	while (*pos < size && skiplist[*pos])
		(*pos)++;
	assert(*pos < size);

	item = splay_entry(node, struct splayitem, splay);
	assert(item->i == *pos);
	(*pos)++;

	check_node_order(node->right, node, skiplist, pos, size);
}

static __inline__ void check_root_order(const struct splay_root *root,
					const uint8_t *skiplist, uint16_t size)
{
	uint16_t pos = 0;

	check_node_order(root->node, NULL, skiplist, &pos, size);

	while (pos < size && skiplist[pos])
		pos++;

	assert(size == pos);
}

#endif /* __SPLAYTREE_COMMON_TREEVALIDATION_H__ */
