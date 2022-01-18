/*
 * 12_3.c Use self-references struct to construct a simple linked list
 *
 *  Created on: 2022年1月18日
 *      Author: ken
 */
#include <stdio.h>
#include <stdlib.h>

struct myNode {
    char letter;
    struct myNode *nextElement;
};

// listNode是struct myNode的別名(alias)
typedef struct myNode listNode;
// nodePtr是指向一個listNode的指標
typedef listNode *nodePtr;

// function prototype
listNode *insert(listNode *sPtr, char value);
void printList(listNode *sPtr);

void main_12_3()
{
	nodePtr startPtr = NULL;
	startPtr = insert(startPtr, 'A');
	startPtr = insert(startPtr, 'B');
	startPtr = insert(startPtr, 'C');
	printList(startPtr);
}

// 在list的最後方插入新值, malloc可能無法成功配置記憶空間,因此得到NULL回傳值,
// 此時的例外處理採取的方式只是印出錯誤而已
listNode *insert(listNode *sPtr, char value) {
	nodePtr newNodePtr = malloc(sizeof(listNode));
	if (newNodePtr != NULL) {
		newNodePtr->letter = value;
	}

	if (sPtr == NULL) {
		sPtr = newNodePtr;
	} else {
		listNode *currentNodePtr = sPtr;
	    if(currentNodePtr->nextElement != NULL) {
	    	currentNodePtr = currentNodePtr->nextElement;
	    }
	    currentNodePtr->nextElement = newNodePtr;
	}
    return sPtr;
}

void printList(listNode *sPtr) {
	if (sPtr == NULL) {
		puts("invalid data");
	}
	listNode *currentNodePtr = sPtr;
	printf("%c", currentNodePtr->letter);
	while(currentNodePtr->nextElement != NULL) {
		currentNodePtr = currentNodePtr->nextElement;
		printf("%c", currentNodePtr->letter);
	}
}

