/*
 * 12_13.c 用C語言實作一個簡單的Queue.
 * 針對Queue有兩種主要動作：enqueue、dequeue.
 * [][][][][][][]......[]
 * |                   |
 * |_Head              |_Tail
 *
 * 需要指向head與tail的兩個指標
 *
 *  Created on: 2022年1月23日
 *      Author: ken
 */

#include <stdio.h>
#include <stdlib.h>

// Queue裡頭的元素
struct queueElement {
	char value;
	struct queueElement *nextElement;
};

// Aliases
typedef struct queueElement dataPoint;
typedef dataPoint *dataPointPtr;

// function prototypes
/**
 * 將一個字元值加到佇列的Tail.
 * @param sPtr 指向佇列的指標
 * @param value 一個字元值
 */
void enqueue(dataPointPtr *sPtr, char value);
/**
 * 從佇列的Head取出一個字元值.
 * @param sPtr 指向佇列的指標
 */
void dequeue(dataPointPtr *sPtr);
void printQueue(dataPointPtr *sPtr);

void main_12_13() {
	dataPointPtr sPtr = NULL;
	char value = NULL;
	unsigned int option = 0;
	// 讓使用者輸入指令,以決定要加值或除值.
	while (option != 3) {
		printf("input option: 1(add), 2(delete) or 3(exit) ");
		scanf("%u", &option);

		switch (option) {
		case 1:
			printf("%s", "Enter a character: ");
			scanf("\n%c", &value);
			enqueue(&sPtr, value);
			printQueue(&sPtr);
			break;
		case 2:
			puts("1");
			break;
		default:
			exit(0);
		}
	}
}

void enqueue(dataPointPtr *sPtr, char value) {
	dataPointPtr newElement = malloc(sizeof(dataPoint));
	// 要處理無法配置記憶體的狀況
	if (newElement == NULL) {
	    printf("%s", "Error! can not allocate new memory.");
	    return;
	}

	newElement->value = value;
	newElement->nextElement = NULL;

	// Queue可能還不存在
	if (*sPtr == NULL) {
		*sPtr = newElement;
	} else {
		// 讓新element成為queue的head
		dataPointPtr tmpPtr = *sPtr;
		*sPtr = newElement;
		(*sPtr)->nextElement = tmpPtr;
	}
}

void printQueue(dataPointPtr *sPtr) {
	if (*sPtr == NULL) {
		puts("Error! queue is null.");
		return;
	}

	dataPointPtr currPointPtr = *sPtr;
	while (currPointPtr != NULL) {
		printf("%c ", currPointPtr->value);
		currPointPtr = currPointPtr->nextElement;
	}
	printf("\n");

}
