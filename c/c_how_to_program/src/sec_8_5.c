/*
 * sec_8_5.c 書本 8.5 節：Standard Input/Output Library Functions
 *  使用getchar(), fgets, sscanf從命令列與陣列讀入資料
 *
 *  Created on: 2021年12月23日
 *      Author: ken
 */

#include <stdio.h>
#include <stdlib.h>

#define SIZE 10

void main_sec_8_5()
{
	char *result;
    int i=0;
    int c=0;

    // read from console input with getchar()
    puts("input a string: ");
   	while((i < SIZE - 1) && (c = getchar()) != '\n') {
   		result[i++] = c;
   	}
   	// 用'\0'結束很重要,若無,result無法列印出來
   	result[i] = '\0';
   	puts(result);

   	// read from console input with fgets and save into an array
   	// char *fgets(char *s, int n, FILE *stream)
   	// stdin也算是一種FILE
   	char inputs[SIZE];
   	puts("Enter a line of text:");
   	fgets(inputs, SIZE, stdin);
   	puts(inputs);
}

