// SPDX-License-Identifier: MIT
/* Minimal Splay-tree helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include <assert.h>

#include "../splaytree.h"
#include "common.h"

int main(void)
{
	struct splayitem item;

	assert(&item == splay_entry(&item.splay, struct splayitem, splay));

	return 0;
}
