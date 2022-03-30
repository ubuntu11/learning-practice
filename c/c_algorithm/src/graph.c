/*
 * graph.c
 * C/C++演算法 2.8 圖結構
 *
 *  Created on: 2022年3月30日
 *      Author: ken
 */

#define MAX_NUMBER 20    // 最多可以有幾個頂點
#define MAX_VALUE  65535 // 權重最大容許值

// 定義圖結構
typedef struct {
	char type;
	int  verticNumber;
	int  edgeNumber;
	char vertics[];
	int  weights[MAX_NUMBER][MAX_NUMBER];
} MyGraph;
