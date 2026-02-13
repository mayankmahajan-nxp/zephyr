/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

int main(void)
{
	printf("Hello World! %s\n", CONFIG_BOARD_TARGET);

	int i = 0;
	while (true) {
		k_sleep(K_MSEC(1000));
		printf("printing hello on console %d\n", i++);
	}

	return 0;
}
