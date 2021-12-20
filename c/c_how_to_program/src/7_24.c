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
const char *SUIT[SUIT_NUMBER] = { "Spades", "Heart", "Diamond", "Club" };
const char *FACE[CARD_NUMER] = { "Ace", "Deuce", "Three", "Four", "Five", "Six",
		"Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King" };
// 書上會用unsigned int來強調它不帶負數
int deal_order[SUIT_NUMBER][CARD_NUMER] = { -1 };

/**
 * 初始化發牌陣列.
 */
void init_deal_order();
/**
 * 透過隨機數洗牌.
 */
void shuffle();
/**
 * 發牌,逐一列印出花色與數字.
 */
void deal();

int main() {
	// 書上根本不用init array,直接宣告 int deal_order[SUIT_NUMBER][CARD_NUMER] = {0}或{-1}即可
	//init_deal_order();
	shuffle();
	deal();
}

void init_deal_order() {
	for (int i = 0; i < SUIT_NUMBER; i++) {
		for (int j = 0; j < CARD_NUMER; j++) {
			deal_order[i][j] = -1;
		}
	}
}

void shuffle() {
	// 對陣列大小,書上習慣用size_t而不是int
	int row = -1;
	int col = -1;
	int order = 0;
	srand(time(NULL));
	// 我的寫法,囉嗦.
	/*
	for (int i = 0; i < SUIT_NUMBER; i++) {
		for (int j = 0; j < CARD_NUMER; j++) {
			RANDOM: row = rand() % 4;
			col = rand() % 13;
			if (deal_order[row][col] < 0){
				deal_order[row][col] = order;
				order++;
			} else {
				goto RANDOM;
			}
		}
	}
	*/

	// 書上的範例用do..while,漂亮多了
	for (int i = 0; i < SUIT_NUMBER * CARD_NUMER; i++) {
		do {
			row = rand() % 4;
			col = rand() % 13;
		} while (deal_order[row][col] > 0);
		deal_order[row][col] = i;
	}
}

void deal() {
	for (int i = 0; i < SUIT_NUMBER * CARD_NUMER; i++) {
		for (int row = 0; row < SUIT_NUMBER; row++) {
			for (int col = 0; col < CARD_NUMER; col++) {
				if (deal_order[row][col] == i) {
					printf("The #%d card is %s of %s\n", i + 1, FACE[col], SUIT[row]);
					continue;
				}
			}
		}
	}
}
