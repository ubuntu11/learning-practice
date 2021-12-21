/*
 * 7_26.c
 *
 *  Created on: 2021年12月21日
 *      Author: ken
 *        Goal: 利用函式指標,根據使用者的指定,以升冪或降冪排序一個整數陣列.
 */

#include <stdio.h>
#include <stdlib.h>
#define INT_ARRAY_SIZE 10

int numbers[INT_ARRAY_SIZE] = { 76, 23, 98, 34, 51, 10, 83, 64, 99, 27 };

int ascending(int a, int b);

int descending(int a, int b);

void bubble_sort();

void swap(int *a, int *b);

int main_7_26() {
	int order;
	printf("0: Ascending, 1: Descending: ");
	scanf("%d", &order);
	// Array預設是pass by reference
	if (order == 0) {
		// 直接在function name前加address operator
		bubble_sort(numbers, &ascending);
	} else {
		bubble_sort(numbers, &descending);
	}

	for (size_t i = 0; i < INT_ARRAY_SIZE; i++) {
		printf("%d\n", numbers[i]);
	}
}

/**
 * 氣泡排序法,需要兩層迴圈,控制好迴園執行次數,可稍微增進點效率.
 * 以函式指標為參數的寫法,基本上就是把它的function prototype抄過來,但是在function name前加*代表指標,
 * function name不用真的是某個function的name,此處只是個參數名而已.
 */
void bubble_sort(int work[], int (*compare)(int a, int b)) {
	for (size_t i = 0; i < INT_ARRAY_SIZE - 1; i++) {
		for (size_t j = 0; j < INT_ARRAY_SIZE - 1 - i; j++) {
			if ((*compare)(work[j], work[j + 1])) {
				swap(&work[j], &work[j + 1]);
			}
		}
	}
}

/*
 * 回傳值為1(True),代表要對調順序,因為是升冪,所以比較條件是a > b.
 */
int ascending(int a, int b) {
	return a > b;
}

/*
 * 回傳值為1(True),代表要對調順序,因為是䧏冪,所以比較條件是a < b.
 */
int descending(int a, int b) {
	return a < b;
}

/**
 * 傳入兩個整數指標,然後交換它們的值,意即記憶體位址.
 */
void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}
