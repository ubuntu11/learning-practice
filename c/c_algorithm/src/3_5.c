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

void findFakeCoinEven(int coins[], int coinsNumber) {
	int a[coinsNumber / 2], b[coinsNumber / 2];
	int weightA=0, weightB=0;
	for (int i=0; i<coinsNumber; i++) {
		if (i < coinsNumber / 2) {
			a[i] = coins[i];
			weightA+= coins[i];
		} else {
			b[i - coinsNumber / 2] = coins[i];
			weightB+= coins[i];
		}
	}

	if (weightA < weightB) {
		findFakeCoin(a, coinsNumber / 2);
	} else {
		findFakeCoin(b, coinsNumber / 2);
	}
}

void findFakeCoinOdd(int coins[], int coinsNumber) {
	int mid = coinsNumber / 2 + 1;
	int a[coinsNumber / 2], b[coinsNumber / 2];
	int weightA=0, weightB=0;
	for (int i=0; i < coinsNumber; i++) {
		if (i < coinsNumber / 2) {
			weightA+= coins[i];
			a[i] = coins[i];
		} else if (i > coinsNumber / 2) {
			weightB+= coins[i];
			b[i] = coins[i - coinsNumber / 2 - 1];
		}
	}

	if (weightA == weightB) {
		printf("coin %d is fake!\n", coinsNumber / 2 + 1);
	}

	if (weightA > weightB) {
		findFakeCoin(b, coinsNumber / 2);
	} else {
		findFakeCoin(a, coinsNumber / 2);
	}
}

void findFakeCoin(int coins[], int coinsNumber) {
	if (coinsNumber % 2 == 0) {
		findFakeCoinEven(coins, coinsNumber);
	} else {
		findFakeCoinOdd(coins, coinsNumber);
	}
}

void main_3_5() {
	int coins[MAX_COINS];
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

	findFakeCoin(coins, n);
}
