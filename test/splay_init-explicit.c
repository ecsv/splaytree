// SPDX-License-Identifier: MIT
/* Minimal Splay-tree helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include <assert.h>

#include "../splaytree.h"

int main(void)
{
	struct splay_root testtree;

	INIT_SPLAY_ROOT(&testtree);
	assert(splay_empty(&testtree));

	return 0;
}
