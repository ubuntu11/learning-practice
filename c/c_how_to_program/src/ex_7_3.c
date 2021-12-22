/*
 * ex_7_3.c Self-Review Exercise 7.3 P348
 *
 *  Created on: 2021年12月22日
 *      Author: ken
 */

#include <stdio.h>
#include <stdlib.h>

#define  SIZE 10

void main_ex_7_3()
{
	float numbers[10] = {0.0, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9};
	float *nPtr = NULL;

	// print array elements using array index
	for (size_t i = 0; i < SIZE; i++) {
		printf("numbers[%d] = %2.4f\n", i, numbers[i]);
	}

	nPtr =  &numbers[0];
	printf("nPtr point to address of numbers[0]: %p\n", nPtr);

	nPtr =  numbers;
	printf("nPtr point to address of numbers[0]: %p\n", nPtr);

	// print array elements using pointer offset, ie: 把指標當成陣列注標
	puts("print array elements using pointer offset");
	for (size_t i = 0; i < SIZE; i++) {
		printf("numbers[%d] = %2.4f\n", i, *(nPtr + i));
	}

	puts("print array elements using pointer offset with array name as pointer name");
	for (size_t i = 0; i < SIZE; i++) {
		printf("numbers[%d] = %2.4f\n", i, *(numbers + i));
	}

	puts("print array elements using pointer index");
	// print array elements using pointer offset, ie: 把指標當成陣列
	for (size_t i = 0; i < SIZE; i++) {
		printf("numbers[%d] = %2.4f\n", i, nPtr[i]);
	}

	// refer to element 4
	// use array index notation
	float element_4 = numbers[3];
	printf("element 4 use array index notation: %2.4f\n", element_4);
	// use pointer/offset with array name
	element_4 = *(numbers + 3);
	printf("use pointer/offset with array name: %2.4f\n", element_4);
	// use pointer/offset with pointer
	element_4 = *(nPtr + 3);
	printf("use pointer/offset with pointer: %2.4f\n", element_4);
	// use pointer/index
	element_4 = nPtr[3];
	printf("use pointer/index: %2.4f\n", element_4);

	// address of nPtr + 8
	printf("address of nPtr + 8: %p\n", nPtr + 8);
	printf("value of nPtr + 8: %2.4f\n", *(nPtr + 8));

	// move nPtr to numbers[5]
	nPtr = &(numbers[5]);
	printf("address of (numbers[5]: %p\n", nPtr);
	printf("value that address: %2.4f\n", *nPtr);
}
