/*
 * 12_8.c 堆疊stack.
 *
 *  Created on: 2022年1月21日
 *      Author: ken
 */
#include <stdio.h>
#include <stdlib.h>

// 定義要放進堆疊內的資料
struct stackNode {
	unsigned int value;
	struct stackNode *nextElement;
};

// 設定別名(alias)
typedef struct stackNode myNode;
typedef myNode *myNodePtr;

// function prototypes
void push(myNodePtr *sPtr, unsigned int value);
unsigned int pop(myNodePtr *sPtr);
void printStack(myNodePtr *sPtr);

void main_12_8() {
	// stack的位址(in fact: address of the 1st node)
	myNodePtr sPtr = NULL;

	unsigned int option = 0;
	unsigned int value;
	// 如何持續等待使用者輸入指令,直到他明確要退出程式？ while(true) ?
	// read user command
	while(option != 3) {
		printf("input your choice 1(push), 2(pop), 3(exit): ");
		scanf("%u", &option);
		switch(option) {
		case 1:
			printf("input an positive integer: ");
			scanf("%u", &value);
			push(&sPtr, value);
			printStack(&sPtr);
			break;
		case 2:
			unsigned int poppedValue = pop(&sPtr);
			printf("get popped value: %u\n", poppedValue);
			printStack(&sPtr);
			break;
		case 3:
			puts("3");
			break;
		}
	}
}


void push(myNodePtr *sPtr, unsigned int value) {
	myNodePtr newNodePtr = malloc(sizeof(myNode));
	newNodePtr->value = value;
	// stack可能仍為空
	if (*sPtr == NULL) {
		// Java是new出一個物件, C是預留一塊記憶體空間
		*sPtr = newNodePtr;
	} else {
		myNodePtr tmpNodePtr = *sPtr;
		*sPtr = newNodePtr;
		newNodePtr->nextElement = tmpNodePtr;
	}
}

unsigned int pop(myNodePtr *sPtr) {
	if (*sPtr == NULL) {
		puts("Error: stack is empty!");
	}

	myNodePtr tmpNodePtr = *sPtr;
	unsigned int value = tmpNodePtr->value;
	*sPtr = (*sPtr)->nextElement;
	free(tmpNodePtr);
	return value;
}

void printStack(myNodePtr *sPtr) {
	if (*sPtr == NULL) {
		printf("Error: stack is empty !");
		return;
	}

	myNodePtr currNodePtr = *sPtr;
	while(currNodePtr != NULL) {
		printf("%u ", currNodePtr->value);
		currNodePtr = currNodePtr->nextElement;
	}
	puts("");
}
