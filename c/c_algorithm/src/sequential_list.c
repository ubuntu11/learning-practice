/*
 *  sequential list: 順序表
 *  使用陣列來儲存同性質、同大小資料的一種表(list)結構
 *  或許可以把它想成是python裡的list.
 *  為什麼要弄個順序表而不直接用Array呢？
 *  因為我們能提供更進階的操作方式而不只是靠array index讀值
 *
 *  Created on: 2022年3月25日
 *      Author: ken
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 100

// 要放進List裡的資料
typedef struct {
	char key[10];
	char name[20];
	int age;
} DATA;

// 順序表
typedef struct {
	DATA listData[MAX_LENGTH + 1];
	int listLength;
} SLType;

// 初始化
void SLInit(SLType *sList) {
	sList->listLength = 0;
}

// 取得順序表表度(資料個數),C的Array並不容易取得陣列大小
int SLlength(SLType *sList) {
	return sList->listLength;
}

// 加入一筆資料
int SLAdd(SLType *sList, DATA data) {
	sList->listData[sList->listLength] = data;
	sList->listLength++;
}

// 在順序表的第n(n >= 0)個位置插入一筆新資料
int SLInsert(SLType *sList, int n, DATA data) {
	// 基本檢查
	//   1: 插入位置超過容量上限
	//   2: 插入位置大於現有筆數
	if (sList->listLength >= MAX_LENGTH || n < 0 || n >= sList->listLength) {
		return 0;
	}

	// ex:目前有三筆資料,指定在第1個位置插入一筆新資料,代表第0筆資料不受影響,第1和2個資料各往
	//    後移一位
	for(int i=sList->listLength - 1; i >= n ; i--) {
		sList->listData[i + 1] = sList->listData[i];
	}
	sList->listData[n] = data;
	sList->listLength++;
}

// 刪除第n(n >= 0)個位置的資料
int SLDelete(SLType *sList, int n) {
	if (n < 0 || n > sList->listLength  - 1) {
			return 0;
	}

	// 自第n+1個位置至結尾的資料各往前移一格
	for (int i=n; i < sList->listLength - 1; i++) {
		sList->listData[i] = sList->listData[i + 1];
	}
	sList->listLength--;
}

DATA *SLFindByNumber(SLType *sList, int n) {
	if (n < 0 || n >= sList->listLength) {
		return NULL;
	}

	return &(sList->listData[n]);
}

int SLFindNumberByKey(SLType *sList, char key[]) {
	for (int i=0; i< sList->listLength; i++) {
		if (strcmp(sList->listData[i].key, key) == 0) {
			return i;
		}
	}

	return 0;
}

void printOut(SLType *sList) {
	printf("list size = %d\n", sList->listLength);
	for(int i=0; i<sList->listLength; i++) {
		printf("%s\n", sList->listData[i].key);
		printf("%s\n", sList->listData[i].name);
	}
}

void main_sequential_list() {
	// 宣告一個SLType變數
	SLType sList;
	// 把它的address傳去進行初始化
	SLInit(&sList);

	DATA data0;
	data0.age=20;
	strcpy( data0.key, "user0" );
	strcpy( data0.name, "Ken Hu" );
	SLAdd(&sList, data0);

	DATA data1;
	data1.age=30;
	strcpy( data1.key, "user1" );
	strcpy( data1.name, "Stefanie Yang" );
	SLAdd(&sList, data1);

	DATA data2;
	data2.age=40;
	strcpy( data2.key, "user2" );
	strcpy( data2.name, "Angela You" );
	SLInsert(&sList, 0, data2);
	printOut(&sList);

	SLDelete(&sList, 0);
	printOut(&sList);

	int pos = SLFindNumberByKey(&sList, "user0");
	printf("User 0 locates at position %d\n", pos);
}

