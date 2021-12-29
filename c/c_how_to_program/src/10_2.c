/*
 * 10_2.c 練基本的structs操作
 *
 *  Created on: 2021年12月29日
 *      Author: ken
 */

#include <stdio.h>

void main_10_2()
{
	struct transformer {
		char id[20];
		float temperature;
		float pressure;
	};

	struct cb {
		char id[20];
		char position;
	} cb1, cb2;

	struct transformer mainTransformer1 = { "XXX", 75.4, 988 };

	puts("structure member operator");
	printf("id of transformer 1= %8s\n", mainTransformer1.id);
	printf("temperature of transformer 1= %3.4f\n", mainTransformer1.temperature);
	printf("pressure of transformer 1= %3.4f\n", mainTransformer1.pressure);

	struct transformer *mainTransformer1Ptr = &mainTransformer1;

	puts("structure pointer operator");
	printf("id of transformer 1= %8s\n",mainTransformer1Ptr->id);
	printf("temperature of transformer 1= %3.4f\n", mainTransformer1Ptr->temperature);
	printf("pressure of transformer 1= %3.4f\n", mainTransformer1Ptr->pressure);
}
