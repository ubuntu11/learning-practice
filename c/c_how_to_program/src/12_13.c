/*
 * 12_13.c 用C語言實作一個簡單的Queue.
 * FIFO：尾入頭出
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
 * @param headPtr 指向佇列頭處的指標
 * @param tailPtr 指向佇列尾處的指標
 * @param value 一個字元值
 */
void enqueue(dataPointPtr *headPtr, dataPointPtr *tailPtr, char value);
/**
 * 從佇列的Head取出一個字元值.
 * @param sPtr 指向佇列的指標
 */
char dequeue(dataPointPtr *headPtr, dataPointPtr *tailPtr);
void printQueue(dataPointPtr *headPtr);

void main_12_13() {
	dataPointPtr headPtr = NULL;
	dataPointPtr tailPtr = NULL;
	char value = ' ';
	unsigned int option = 0;
	// 讓使用者輸入指令,以決定要加值或除值.
	while (option != 3) {
		printf("input option: 1(add), 2(delete) or 3(exit) ");
		scanf("%u", &option);

		switch (option) {
		case 1:
			printf("%s", "Enter a character: ");
			scanf("\n%c", &value);
			enqueue(&headPtr, &tailPtr, value);
			printQueue(&headPtr);
			break;
		case 2:
			value = dequeue(&headPtr, &tailPtr);
			printf("value of head: %c\n", value);
			printQueue(&headPtr);
			break;
		default:
			exit(0);
		}
	}
}

void enqueue(dataPointPtr *headPtr, dataPointPtr *tailPtr, char value) {
	dataPointPtr newElement = malloc(sizeof(dataPoint));
	// 要處理無法配置記憶體的狀況
	if (newElement == NULL) {
	    printf("%s", "Error! can not allocate new memory.");
	    return;
	}

	newElement->value = value;
	newElement->nextElement = NULL;

	// Queue可能還不存在
	if (*tailPtr == NULL) {
		*tailPtr = newElement;
		*headPtr = newElement;
	} else {
		// 讓新element成為queue的tail
		dataPointPtr tmpPtr = *tailPtr;
		*tailPtr = newElement;
		tmpPtr->nextElement = *tailPtr;
	}
}

char dequeue(dataPointPtr *headPtr, dataPointPtr *tailPtr) {
	if (*headPtr == NULL) {
		printf("%s\n", "Error! queue is empty.");
		return ' ';
	}

	char value = (*headPtr)->value;
	// 頭尾指向同一個element
	if (*headPtr == *tailPtr) {
		*headPtr = NULL;
		*tailPtr = NULL;
	} else {
		dataPointPtr tmpPtr = *headPtr;
		*headPtr = (*headPtr)->nextElement;
		free(tmpPtr);
	}

	return value;
}

void printQueue(dataPointPtr *headPtr) {
	if (*headPtr == NULL) {
		puts("Error! queue is null.");
		return;
	}

	dataPointPtr currPointPtr = *headPtr;
	while (currPointPtr != NULL) {
		printf("%c ", currPointPtr->value);
		currPointPtr = currPointPtr->nextElement;
	}
	printf("\n");

}
