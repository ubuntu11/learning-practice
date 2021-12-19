/*
 * 7_24.c
 * 使用指標的撲克牌洗牌與發牌程式.
 *
 *  Created on: 2021年12月19日
 *      Author: ken
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SUIT_NUMBER 4
#define CARD_NUMER  13

// 和Java一樣, '用在字元, "用在字串, 陣列初始值用 {}
const char *SUIT[SUIT_NUMBER] = { "spades", "heart", "diamond", "club" };
const char *FACE[CARD_NUMER] = { "Ace", "Deuce", "Three", "Four", "Five", "Six",
		"Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King" };
int deal_order[SUIT_NUMBER][CARD_NUMER];

// function prototypes
void init_deal_order();
void shuffle();

int main() {
	init_deal_order();
	srand(time(NULL));
	int i = rand();
	printf("%d", i);
}

void init_deal_order() {
	for (int i = 0; i < SUIT_NUMBER; i++) {
		for (int j = 0; j < CARD_NUMER; j++) {
			deal_order[i][j] = 0;
		}
	}
}

void shuffle() {

}
