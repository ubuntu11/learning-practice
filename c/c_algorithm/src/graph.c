/*
 * graph.c
 * C/C++演算法 2.8 圖結構.
 * 用一個一維陣列儲存頂點名,一個二維陣列儲存各邊的權重
 *
 *  Created on: 2022年3月30日
 *      Author: ken
 */
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUMBER 20    // 最多可以有幾個頂點
#define MAX_VALUE  65535 // 權重最大容許值

// 定義圖結構
typedef struct {
	char type;         // 圖類型,0無向,1有向
	int  verticNumber; // 幾個頂點
	int  edgeNumber;   // 幾個邊
	char vertics[MAX_NUMBER];    // 頂點名
	int  weights[MAX_NUMBER][MAX_NUMBER]; // 各邊的權重
} MyGraph;

// function prototypes
void createGraph(MyGraph *g);

void main_graph() {
	MyGraph g;
	g.type = 0;
	g.verticNumber = 3;
	g.edgeNumber = 3;
	createGraph(&g);
}

/**
 * 讀取使用者在console的輸入值,組建一個圖結構.輸入值包括所有頂點與所有邊的權重,而圖類型、頂點
 * 數及邊數量已在主程式中指定.
 */
void createGraph(MyGraph *g) {
	char startv, endv;
	int weight;

	for (int i = 0; i < g->verticNumber; i++) {
		getchar();
		printf("name of vertex %d: \n", i);
		scanf("%c", &(g->vertics[i]));
	}

	for (int i = 0; i < g->edgeNumber; i++) {
		getchar();
		printf("edge weight between 2 vertics\n");
		scanf("%c %c %d", &startv, &endv, &weight);
		// char不能用%s列印: segmentation fault
		printf("%s\n", startv);
	}
}
