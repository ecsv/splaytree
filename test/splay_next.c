// SPDX-License-Identifier: MIT
/* Minimal Splay-tree helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2019, Sven Eckelmann <sven@narfation.org>
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "../splaytree.h"
#include "common.h"
#include "common-treeops.h"

static uint16_t values[256];

static struct splayitem items[ARRAY_SIZE(values)];

int main(void)
{
	struct splay_root root;
	struct splay_node *node;
	struct splayitem *item;
	size_t i, j;

	INIT_SPLAY_ROOT(&root);
	items[0].i = 0;
	splayitem_insert_unbalanced(&root, &items[0]);
	assert(splay_next(&items[0].splay) == NULL);

	for (i = 0; i < 256; i++) {
		random_shuffle_array(values, (uint16_t)ARRAY_SIZE(values));

		INIT_SPLAY_ROOT(&root);
		node = splay_first(&root);
		assert(!node);

		for (j = 0; j < ARRAY_SIZE(values); j++) {
			items[j].i = values[j];
			splayitem_insert_unbalanced(&root, &items[j]);
		}

		for (node = splay_first(&root), j = 0;
		     node;
		     j++, node = splay_next(node)) {
			item = splay_entry(node, struct splayitem, splay);
			assert(item->i == j);
		}
		assert(j == ARRAY_SIZE(values));
		assert(!node);
	}

	return 0;
}
