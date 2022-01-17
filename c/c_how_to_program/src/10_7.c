/*
 * 10_7.c display an int in digits
 *
 *  Created on: 2022年1月17日
 *      Author: ken
 */

#include <stdio.h>
#include <limits.h> // we need CHAR_BIT constant

// function prototype
void displayDigits(unsigned int);

void main_10_7() {
	unsigned int a = 15;
	displayDigits(a);
}

void displayDigits(unsigned int value) {
	int bitsOfInt = sizeof(unsigned int) * CHAR_BIT;
	// 先算出要把當做遮罩用的整數1所有bits往左位移多少
	unsigned int mask = 1 << (bitsOfInt - 1);
	//   15 = 000000000 00000000 00000000 00001111
	// mask = 100000000 00000000 00000000 00000000
	// AND 只有值bit與mask首bit同為1者會列印出來
	// 每次用最高位bit和mask首bit做比較,因此value要次次向左shift一個bit
	for (int i = 0; i < bitsOfInt; i++) {
		putchar(value & mask ? '1' : '0');
		if (i != 0 && i % 8 == 0) {
			printf("%s", " ");
		}
		value <<= 1;
	}
}
