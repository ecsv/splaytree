/* SPDX-License-Identifier: MIT */
/* Minimal Splay-tree helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2019, Sven Eckelmann <sven@narfation.org>
 */

#ifndef __SPLAYTREE_COMMON_PRIOQUEUE_H__
#define __SPLAYTREE_COMMON_PRIOQUEUE_H__

#include <stdbool.h>
#include <stddef.h>

#include "../splaytree.h"
#include "common.h"

struct splay_prioqueue {
	struct splay_root root;
	struct splay_node *min_node;
};

static __inline__ void splay_prioqueue_init(struct splay_prioqueue *queue)
{
	INIT_SPLAY_ROOT(&queue->root);
	queue->min_node = NULL;
}

static __inline__ void
splay_prioqueue_insert_unbalanced(struct splay_prioqueue *queue,
				  struct splayitem *new_entry)
{
	struct splay_node *parent = NULL;
	struct splay_node **cur_nodep = &queue->root.node;
	struct splayitem *cur_entry;
	int isminimal = 1;

	while (*cur_nodep) {
		cur_entry = splay_entry(*cur_nodep, struct splayitem, splay);

		parent = *cur_nodep;
		if (cmpint(&new_entry->i, &cur_entry->i) <= 0) {
			cur_nodep = &((*cur_nodep)->left);
		} else {
			cur_nodep = &((*cur_nodep)->right);
			isminimal = 0;
		}
	}

	if (isminimal)
		queue->min_node = &new_entry->splay;

	splay_link_node(&new_entry->splay, parent, cur_nodep);
}

static __inline__ struct splayitem *
splay_prioqueue_pop_unbalanced(struct splay_prioqueue *queue)
{
	struct splayitem *item;

	if (!queue->min_node)
		return NULL;

	item = splay_entry(queue->min_node, struct splayitem, splay);
	queue->min_node = splay_next(queue->min_node);

	splay_erase_node(&item->splay, &queue->root);

	if (queue->min_node)
		splay_splaying(queue->min_node, &queue->root);

	return item;
}

static __inline__ void
splay_prioqueue_insert_balanced(struct splay_prioqueue *queue,
				struct splayitem *new_entry)
{
	struct splay_node *parent = NULL;
	struct splay_node **cur_nodep = &queue->root.node;
	struct splayitem *cur_entry;
	int isminimal = 1;

	while (*cur_nodep) {
		cur_entry = splay_entry(*cur_nodep, struct splayitem, splay);

		parent = *cur_nodep;
		if (cmpint(&new_entry->i, &cur_entry->i) <= 0) {
			cur_nodep = &((*cur_nodep)->left);
		} else {
			cur_nodep = &((*cur_nodep)->right);
			isminimal = 0;
		}
	}

	if (isminimal)
		queue->min_node = &new_entry->splay;

	splay_insert(&new_entry->splay, parent, cur_nodep, &queue->root);
}

static __inline__ struct splayitem *
splay_prioqueue_pop_balanced(struct splay_prioqueue *queue)
{
	struct splayitem *item;

	if (!queue->min_node)
		return NULL;

	item = splay_entry(queue->min_node, struct splayitem, splay);
	queue->min_node = splay_next(queue->min_node);

	splay_erase(&item->splay, &queue->root);

	return item;
}

#endif /* __SPLAYTREE_COMMON_PRIOQUEUE_H__ */
