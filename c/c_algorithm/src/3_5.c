/*
 * 3_5.c  分治(divide and conquer)演算法.
 * 找出假硬幣問題.
 *
 *  Created on: 2022年4月18日
 *      Author: ken
 */

#include <stdio.h>
#include <stdlib.h>
#define MAX_COINS 30

int coins[MAX_COINS];

/**
 *
 * lowerLimit: 要處理的硬幣群中最小的陣列索引值(最小為0)
 * upperLimit: 要處理的硬幣群中最大的陣列索引值(最大為陣列大小-1)
 */
void findFakeCoin(int lowerLimit, int upperLimit) {
	int size, mid;

	size = upperLimit - lowerLimit + 1;
	mid = size / 2;

	int a[mid], b[mid];
	for (int i=lowerLimit; i<=upperLimit; i++) {
        if (i < mid) {
        	a[i] = coins[i];
        	printf("a[%d]=%d\n",i, coins[i]);
        } else {
        	if (size % 2 == 0) {
        		b[i - mid] = coins[i];
        		printf("b[%d]=%d\n",i - mid, coins[i]);
        	} else {
        		if (i == mid) {
        			continue;
        		}
        		b[i - mid - 1] = coins[i];
        		printf("b[%d]=%d\n",i - mid - 1, coins[i]);
        	}
        }
	}
}

void main_3_5() {
	int n=0;
	printf("input how many coins? ");
	scanf("%d", &n);

	if (n > MAX_COINS) {
		printf("too many coins! the maximum value is %d\n", MAX_COINS);
		exit(-1);
	}

	for(int i=0; i<n; i++) {
		printf("input coin %d (1 False, 2 True)=", (i + 1));
		scanf("%d", &coins[i]);
	}

	findFakeCoin(0, n - 1);
}
