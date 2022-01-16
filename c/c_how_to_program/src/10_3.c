/*
 * 10_3.c
 * C How to program, P444-445. A high performance card shuffling and dealing program.
 *
 *  Created on: 2022年1月16日
 *      Author: ken
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 定義兩個常數, 共幾張牌, 有幾種不同面值.
#define CARDS 52
#define FACES 13

// 定義代表poker card的struct type
typedef struct {
  char *suit;
  char *face;
} Card;

// 代表整副撲克牌的陣列
Card deck[CARDS];
// 四種花色
char *suits[] = {"SPADE", "HEART", "DIAMOND", "CLUB"};
// 十三種面值
char *faces[] = {"Ace", "Deuce", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King"};

// function prototypes
void initCards();
void shuffle_10_3();

void main_10_3()
{
	srand(time(NULL));

    initCards();
    shuffle_10_3();
    for(size_t i=0; i< CARDS; i++) {
    	printf("suit: %s - face: %s\n", deck[i].suit, deck[i].face);
    }
}

void initCards()
{
  for(size_t i=0; i< CARDS; i++) {
	  deck[i].suit = suits[i / 13];
	  deck[i].face = faces[i % 13];
  }
}

// 跑一次1-52的迴圈,每次取一個亂數值,用陣列中當前迴圈index與此亂數值所對應的元素做交換
void shuffle_10_3()
{
	for(size_t i=0; i< CARDS; i++) {
		  size_t j = rand() % CARDS;
		  Card _temp = deck[i];
		  deck[i] = deck[j];
		  deck[j] = _temp;
	  }
}

