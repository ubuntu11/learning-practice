/*
 * 12_19.c
 * 實作一個簡單的二元搜尋樹(Binary Search Tree),產生十個隨機數置於其上,然後用中序、前序、後序
 * 來遍歷這棵樹.
 *
 *  Created on: 2022年1月25日
 *      Author: ken
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 樹上每個節點除了節點值(一個整數)外,用兩個指標指向其左側與右側的節點.
struct treeNode {
	struct treeNode *leftNodePtr;
	unsigned int nodeValue;
	struct treeNode *rightNodePtr;
};

// alias(synonym)
typedef struct treeNode myNode;
typedef myNode *myNodePtr; //指向一個myNode的指標

// function prototypes
void addValue(myNodePtr *sPtr, int nodeValue); //sPtr是指向一個myNodePtr的指標
void inOrder(myNodePtr sPtr); //中序: 先印中
void preOrder(myNodePtr sPtr); //前序: 先印左
void postOrder(myNodePtr sPtr); //後序: 先印右

void main_12_19()
{
	// 指向樹的指標(其實是指向樹上根節點的指標)
	myNodePtr sPtr = NULL;
	/*
	 *            27
	 *               45
	 */
	int testNumbers[] = {27, 45, 13, 9, 64, 57, 83, 19, 7, 37};

	// 產生十個隨機數
	srand(time(NULL));
	int nodeValue = -1;
	for(size_t i=0; i<10; i++) {
		nodeValue = rand() % 100;
		//nodeValue = testNumbers[i];
		addValue(&sPtr, nodeValue);
	}
	inOrder(sPtr);
	puts("");
	preOrder(sPtr);
	puts("");
	postOrder(sPtr);
	puts("");
}

void addValue(myNodePtr *sPtr, int nodeValue) {
	myNodePtr newNodePtr = malloc(sizeof(myNode));

	if (newNodePtr != NULL) {
		newNodePtr->nodeValue = nodeValue;
		if (*sPtr == NULL) {
			*sPtr = newNodePtr;
			(*sPtr)->leftNodePtr = NULL;
			(*sPtr)->rightNodePtr = NULL;
		} else {
			if ((*sPtr)->nodeValue < nodeValue) {
				addValue(&((*sPtr)->rightNodePtr), nodeValue);
			} else if ((*sPtr)->nodeValue > nodeValue){
				addValue(&((*sPtr)->leftNodePtr), nodeValue);
			} else {
				printf("ignore duplicated value %d\n", nodeValue);
			}
		}
	} else {
		printf("%s\n", "Error! can not allocate memory.");
	}
}

void inOrder(myNodePtr sPtr) {
	if (sPtr != NULL) {
		inOrder(sPtr->leftNodePtr);
		printf("%3d", sPtr->nodeValue);
		inOrder(sPtr->rightNodePtr);
	}
}

void preOrder(myNodePtr sPtr) {
	if (sPtr != NULL) {
		printf("%3d", sPtr->nodeValue);
		preOrder(sPtr->leftNodePtr);
		preOrder(sPtr->rightNodePtr);
	}
}

void postOrder(myNodePtr sPtr) {
	if (sPtr != NULL) {
		postOrder(sPtr->leftNodePtr);
		postOrder(sPtr->rightNodePtr);
		printf("%3d", sPtr->nodeValue);
	}
}
