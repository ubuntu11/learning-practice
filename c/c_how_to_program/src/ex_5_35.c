/*
 * ex_5_35.c A nonrecursive version of fibonacci series.
 *
 *  Created on: 2022年2月8日
 *      Author: ken
 *
 *  Fibonacci series: 0 1 1 2 3 5 8 ...
 */

#include <stdio.h>
#include <stdlib.h>

unsigned long long getNthNumber(unsigned int);

void main_ex_5_35() {
	unsigned int n = 1000;
	unsigned long long result = getNthNumber(n);
	printf("the %dth number is %llu", n, result);
}

/**
 * Fibonacci數列中的第N個數字(從1起算).
 */
unsigned long long getNthNumber(unsigned int n) {
	unsigned int baseNumbers[2] = {0, 1};
	unsigned long long sum = 0;
	if (n < 2) {
		return baseNumbers[n - 1];
	}

	// n=3時做1次加法, n=4時做2次加法
	for(int i=0; i < n - 2 ; i++) {
		sum = baseNumbers[0] + baseNumbers[1];
		baseNumbers[0] = baseNumbers[1];
		baseNumbers[1] = sum;
	}

	return sum;
}
