/*
 * mms_server.c
 *
 *  Created on: 2022年2月3日
 *      Author: ken
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define PI 3.1415926

// 報價代碼聚合之所有交易資源加總之瞬時輸出/入總實功率
struct GroupRealPowerStruc {
	int totalWatt;
	unsigned long timestamp;
};
typedef struct GroupRealPowerStruc GroupRealPower;

// DI and AI of 報價代碼
struct GroupInputStruc {
	// 履行服務開始
	bool serviceStart;
	// 履行服務結束
	bool serviceStop;
	// 合格交易者代碼
	unsigned int providerId;
	// 報價代碼
	unsigned int quotationId;
	// 輔助服務商品
	unsigned char serviceCode;
	// xxx
	GroupRealPower realPower[10];
	// 報價代碼聚合之所有交易資源加總瞬時累計發電量
	unsigned long supplyWatt;
	// 報價代碼聚合之所有交易資源加總瞬時累計用電量
	unsigned long demandWatt;
	// 執行率計算時間點
	unsigned long timestamp;
	// 執行率
	unsigned int executionRate;
};
typedef struct GroupInputStruc GroupInput;

// function prototypes
void simulate();

int main() {
	simulate();
}

void simulate() {
	GroupInput *groupInput = malloc(sizeof(GroupInput));
	groupInput->serviceStart = true;
	groupInput->serviceStop = false;
	groupInput->providerId = 8477;
	groupInput->quotationId = 8623;
	groupInput->serviceCode = 0;
	groupInput->supplyWatt = 12000;
	groupInput->demandWatt = 10000;
	groupInput->timestamp = (unsigned long)time(NULL);
	groupInput->executionRate = 8888;
	for(int i=0; i<10; i++) {
		GroupRealPower *groupRealPower = malloc(sizeof(GroupRealPower));
		groupRealPower->totalWatt = 10000 - 1000 * sin(2 * PI * (40 * i) / 360);
		groupRealPower->timestamp = groupInput->timestamp - (10 - i);
		groupInput->realPower[i] = *groupRealPower;
	}

	printf("%21s: %d\n", "service start", groupInput->serviceStart);
	printf("%21s: %d\n", "service stop", groupInput->serviceStop);
	printf("%21s: %05d\n", "provider id", groupInput->providerId);
	printf("%21s: %05d\n", "quotation id", groupInput->quotationId);
	printf("%21s: %5d\n", "service code", groupInput->serviceCode);
	printf("%21s: %5ld\n", "service code", groupInput->supplyWatt);
	printf("%21s: %5ld\n", "service code", groupInput->demandWatt);
	for(int i=0; i<10; i++) {
		printf("real power at sec. %02d: %5d\n", i + 1, groupInput->realPower[i].totalWatt);
		printf("%21s: %ld\n", "timestamp", groupInput->realPower[i].timestamp);
	}
	printf("%21s: %5d\n", "execution rate", groupInput->executionRate);
	printf("%21s: %ld\n", "timestamp", groupInput->timestamp);
	free(groupInput);
}
