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
void clearGraph(MyGraph *g);

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
	int weight, idx1, idx2;

	for (int i = 0; i < g->verticNumber; i++) {
		getchar();
		printf("name of vertex %d: \n", i);
		scanf("%c", &(g->vertics[i]));
	}

	for (int i = 0; i < g->edgeNumber; i++) {
		getchar();
		printf("edge weight between 2 vertics\n");
		scanf("%c %c %d", &startv, &endv, &weight);
		// 找出頂點名稱所對應的陣列index
		for(idx1=0;g->vertics[idx1] != startv; idx1++);
		for(idx2=0;g->vertics[idx2] != endv; idx2++);
		g->weights[idx1][idx2] = weight;
		// 無向圖,矩陣值在對角線兩側為對稱
		if (g->type ==0) {
			g->weights[idx2][idx1] = weight;
		}
	}
}

/**
 * 把圖結構清空,將矩陣上各cell值設為我們選用的Max value.
 */
void clearGraph(MyGraph *g) {
	for (int i=0; i < g->verticNumber; i++) {
		for (int j=0; j < g->verticNumber; j++) {
			g->weights[i][j] = MAX_VALUE;
		}
	}
}
