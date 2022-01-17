/*
 * 10_18.c enum in C
 *
 *  Created on: 2022年1月17日
 *      Author: ken
 */

#include <stdio.h>

void main_10_18() {
	enum months {
		JAN = 1, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC
	};

	// 存放各月份全名的字元陣列
	char *monthNames[] = { "", "January", "February", "March", "April", "May",
			"June", "July", "August", "September", "October", "November",
			"December" };

	// 注意這裡loop裡的寫法
	for(enum months month = JAN; month <= DEC; month++) {
		printf("%3d%12s\n", month, monthNames[month]);
	}
}

