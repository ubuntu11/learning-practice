/*
 * ex_8_11.c Self-Review Exercise 8.11 P403
 *
 *  Created on: 2021年12月25日
 *      Author: ken
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 又弄錯！C裡的字串其實是一個字元陣列,而陣列名稱其實是首元素記憶體位址,意即字串可用字元指標表示.
// 因此, 把多個字串放進一個陣列中,那個陣列就是一個字元指標陣列.
// 宣告字串陣列變數時必須使用char *NAME[], 而不能用char **NAME
char *article[] = { "the", "a", "one", "some", "any" };
char *noun[] = { "boy", "girl", "dog", "town", "car" };
char *verb[] = { "drove", "jumped", "ran", "walked", "skipped" };
char *preposition[] = { "to", "from", "over", "under", "on" };
char *candidate[] = { article, noun, verb, preposition, article, noun};

char* pick_str(char **candidate, size_t size);
/*
 * 依以下順序從陣列中隨機取出一字使其成為一個句子,首字字母應大寫,句末應加上句號.
 *
 * 順序：article noun verb preposition article noun
 */
void main_ex_8_11() {
	char result[100] = "";
	srand(time(NULL));
	for (size_t i = 0; i < 6; i++) {
		strcat(result, pick_str(candidate[i], 5));
		strcat(result, " ");
	}
	printf("final result: %s\n", result);
}

// *candidate代表一個字串, **candidate代表一個字串陣列
char* pick_str(char **candidate, size_t size) {
	int index = rand() % (size - 1);
	return candidate[index];
}
