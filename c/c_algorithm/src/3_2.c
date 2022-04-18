/*
 * 3_1.c 窮舉法(exhaustive attack method)
 * 35頭,949足之雞免同籠題.
 *
 *  Created on: 2022年4月18日
 *      Author: ken
 */

#include <stdio.h>
#include <stdlib.h>

void main_3_2() {
	int x; //雞數
	int y; //兔數

	for(x=0; x<=35; x++) {
		y = 35 - x;
		if ( x * 2 + y * 4 == 94) {
			printf("雞 has %d, 兔 has %d\n", x, y);
			break;
		}
	}
}

