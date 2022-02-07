/*
 * ex_5_36.c Tower of Hanoi.
 *
 *  Created on: 2022年2月7日
 *      Author: ken
 */

#include <stdio.h>
#include <stdlib.h>

// function prototype
void moveDisks(unsigned short disks, char pegFrom, char pegTo, char pegTemp);

void main_ex_5_36() {
	moveDisks(3, 1, 2, 3);
}

void moveDisks(unsigned short disks, char pegFrom, char pegTo, char pegTemp) {
	if (disks == 1) {
		printf("%d-%d\n", pegFrom, pegTo);
		return;
	} else {
		printf("%d-%d\n", pegFrom, pegTemp);
		printf("%d-%d\n", pegFrom, pegTo);
		printf("%d-%d\n", pegTemp, pegTo);
		disks--;
		moveDisks(disks, pegTo, pegTemp, pegFrom);
	}
}
