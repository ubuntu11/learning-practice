/*
 * ex_5_36.c Tower of Hanoi.
 *
 *  Created on: 2022年2月7日
 *      Author: ken
 *
 *  2 disks with 3 pegs:
 *    1-2
 *    1-3
 *    2-3
 *
 *  3 disks with 3 pegs:
 *    1-3
 *    1-2
 *    3-2
 *    1-3
 *    2-1
 *    2-3
 *    1-3
 *
 *  4 disks with 3 pegs:
 *    f(4) from 1 to 3 =
 *       f(3) from 1 to 2 +
 *       1-3              +
 *       f(3) from 2 to 3
 */

#include <stdio.h>
#include <stdlib.h>

// function prototype
void moveDisks(unsigned short disks, char pegFrom, char pegTo, char pegTemp);

void main_ex_5_36() {
	moveDisks(3, 1, 3, 2);
}

void moveDisks(unsigned short disks, char pegFrom, char pegTo, char pegTemp) {
	if (disks == 2) {
		printf("%d-%d\n", pegFrom, pegTemp);
		printf("%d-%d\n", pegFrom, pegTo);
		printf("%d-%d\n", pegTemp, pegTo);
		return;
	} else {
		disks--;
		moveDisks(disks, pegFrom, pegTemp, pegTo);
		printf("%d-%d\n", pegFrom, pegTo);
		moveDisks(disks, pegTemp, pegTo, pegFrom);
	}
}
