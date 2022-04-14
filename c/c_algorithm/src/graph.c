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
	int type;         // 圖類型,0無向,1有向
	int  verticNumber; // 幾個頂點
	int  edgeNumber;   // 幾個邊
	char vertics[MAX_NUMBER];    // 頂點名
	int  weights[MAX_NUMBER][MAX_NUMBER]; // 各邊的權重
	int  isTraversed[MAX_NUMBER]; //遍歷時,某頂點是否訪問過
} MyGraph;

// function prototypes
void createGraph(MyGraph *g);
void clearGraph(MyGraph *g);
void printOutGraph(MyGraph *g);
void traverseGraph(MyGraph *g);

void main_graph() {
	MyGraph g;
	printf("input graph type(0: undirected, 1: directed");
	scanf("%d", &(g.type));
	printf("input number of vertices");
	scanf("%d", &(g.verticNumber));
	printf("input number of edges");
	scanf("%d", &(g.edgeNumber));

	clearGraph(&g);
	createGraph(&g);
	printOutGraph(&g);
	traverseGraph(&g);
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

/**
 * 把圖結構用矩陣的型式列印出來, ex:
 *    A B C
 *  A Z 1 1
 *  B Z Z 1
 *  C Z Z Z
 *
 *  Z代表MAX VALUE,代表沒有連成邊
 */
void printOutGraph(MyGraph *g) {
	for (int i=0; i< g->verticNumber; i++) {
		printf("\t%c", g->vertics[i]);
	}
	printf("\n");

	for (int i=0; i< g->verticNumber; i++) {
		printf("%c", g->vertics[i]);
		for (int j=0; j< g->verticNumber; j++) {
			if (g->weights[i][j] == MAX_VALUE) {
				printf("\t%s", "Z");
			} else {
				printf("\t%d", g->weights[i][j]);
			}
		}
		printf("\n");
	}
}

/**
 * 以某個頂點為起點,以深度優先方式遍歷與其相連的頂點.
 */
void visitOneVertex(MyGraph *g, int vIdx) {
	g->isTraversed[vIdx] = 1;
	printf("->%c", g->vertics[vIdx]);

	for (int i=0; i< g->verticNumber; i++) {
		if (g->weights[vIdx][i] != MAX_VALUE && g->isTraversed[i] != 1) {
			visitOneVertex(g, i);
		}
	}
}

/**
 * 基於深度優先方式遍歷圖結構, 訪問過的頂點會在isTraversed陣列中標為1.
 */
void traverseGraph(MyGraph *g) {
	for (int i=0; i < g->verticNumber; i++) {
		g->isTraversed[i] = 0;
	}

	for (int i=0; i< g->verticNumber; i++) {
		if (g->isTraversed[i] == 0) {
			visitOneVertex(g, i);
		}
	}
	printf("\n");
}
