/*
 * 3_2.c 遞推算法.
 * 以Fibonacci數列為例: 1 1 2 3 5 8 13 21 ....
 * 根據已知結果和關係求解中間結果,判定是否達到要求,有則停止,無則繼續遞推.
 *
 *  Created on: 2022年4月18日
 *      Author: ken
 */

#include <stdio.h>
#include <stdlib.h>

int fibonacci(int n) {
	if (n <= 2) {
		return 1;
	} else {
		return fibonacci(n - 1) + fibonacci(n - 2);
	}
}

void main_3_3() {
	int n=0;
	getchar();
	printf("input n=");
	scanf("%d", &n);
	int f = fibonacci(n);
	printf("fibonacci number of %d is %d\n", n, f);
}
