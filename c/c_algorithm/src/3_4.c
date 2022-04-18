/*
 * 3_3.c 遞迴演算法.
 *
 *  直接遞迴: Func A呼叫Func A自己
 *  間接遞迴: Func A呼叫Func B, Func B呼叫Func C, Func C呼叫Func A
 *  求數字階乘.
 *  Created on: 2022年4月18日
 *      Author: ken
 */

#include <stdio.h>
#include <stdlib.h>

int factorial(int n) {
	if (n ==0 || n ==1) {
		return 1;
	} else {
		return n * factorial(n - 1);
	}
}

void main_3_4() {
	int n=0;
	printf("input n=");
	scanf("%d", &n);
	printf("factorial(%d) = %d\n", n , factorial(n));
}
