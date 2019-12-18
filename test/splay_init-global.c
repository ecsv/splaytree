// SPDX-License-Identifier: MIT
/* Minimal Splay-tree helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2019, Sven Eckelmann <sven@narfation.org>
 */

#include <assert.h>

#include "../splaytree.h"

static DEFINE_SPLAYROOT(testtree);

int main(void)
{
	assert(splay_empty(&testtree));

	return 0;
}
