/* SPDX-License-Identifier: MIT */
/* Minimal Splay-tree helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2019, Sven Eckelmann <sven@narfation.org>
 */

#ifndef __SPLAYTREE_COMMON_TREEOPS_H__
#define __SPLAYTREE_COMMON_TREEOPS_H__

#include <stddef.h>
#include <stdint.h>

#include "../splaytree.h"
#include "common.h"

static __inline__ void splayitem_insert_unbalanced(struct splay_root *root,
						   struct splayitem *new_entry)
{
	struct splay_node *parent = NULL;
	struct splay_node **cur_nodep = &root->node;
	struct splayitem *cur_entry;

	while (*cur_nodep) {
		cur_entry = splay_entry(*cur_nodep, struct splayitem, splay);

		parent = *cur_nodep;
		if (cmpint(&new_entry->i, &cur_entry->i) <= 0)
			cur_nodep = &((*cur_nodep)->left);
		else
			cur_nodep = &((*cur_nodep)->right);
	}

	splay_link_node(&new_entry->splay, parent, cur_nodep);
}

static __inline__ void splayitem_insert_balanced(struct splay_root *root,
						 struct splayitem *new_entry)
{
	splayitem_insert_unbalanced(root, new_entry);
	splay_splaying(&new_entry->splay, root);
}

static __inline__ struct splayitem *splayitem_find(struct splay_root *root,
						   uint16_t x)
{
	struct splay_node **cur_nodep = &root->node;
	struct splayitem *cur_entry;
	int res;

	while (*cur_nodep) {
		cur_entry = splay_entry(*cur_nodep, struct splayitem, splay);

		res = cmpint(&x, &cur_entry->i);
		if (res == 0)
			return cur_entry;

		if (res < 0)
			cur_nodep = &((*cur_nodep)->left);
		else
			cur_nodep = &((*cur_nodep)->right);
	}

	return NULL;
}

#endif /* __SPLAYTREE_COMMON_TREEOPS_H__ */
