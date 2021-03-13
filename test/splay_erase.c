// SPDX-License-Identifier: MIT
/* Minimal Splay-tree helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../splaytree.h"
#include "common.h"
#include "common-treeops.h"
#include "common-treevalidation.h"

static uint16_t values[256];
static uint16_t delete_items[ARRAY_SIZE(values)];
static uint8_t skiplist[ARRAY_SIZE(values)];

int main(void)
{
	struct splay_root root;
	size_t i, j;
	struct splayitem *item;

	for (i = 0; i < 256; i++) {
		random_shuffle_array(values, (uint16_t)ARRAY_SIZE(values));
		memset(skiplist, 1, sizeof(skiplist));

		INIT_SPLAY_ROOT(&root);
		for (j = 0; j < ARRAY_SIZE(values); j++) {
			item = (struct splayitem *)malloc(sizeof(*item));
			assert(item);

			item->i = values[j];
			splayitem_insert_balanced(&root, item);
			skiplist[values[j]] = 0;
		}

		random_shuffle_array(delete_items, (uint16_t)ARRAY_SIZE(delete_items));
		for (j = 0; j < ARRAY_SIZE(delete_items); j++) {
			item = splayitem_find(&root, delete_items[j]);

			assert(item);
			assert(item->i == delete_items[j]);

			splay_erase(&item->splay, &root);
			skiplist[item->i] = 1;
			free(item);

			check_root_order(&root, skiplist,
					(uint16_t)ARRAY_SIZE(skiplist));
		}
		assert(splay_empty(&root));
	}

	return 0;
}
