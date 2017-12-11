#include "os.h"
#include <stdio.h>
#include "utils/serial.h"
#include "sleep.h"
#include "fixedPriorityScheduler.h"
#include "stm32f4xx.h"
void task1(void const *const args) {
	for (int i = 0; i < 12; ++i) {
		printf("A");
	}
}

void task2(void const *const args) {
	for (int i = 0; i < 12; ++i) {
		printf("B");
	}
}

void task3(void const *const args) {
	for (int i = 0; i < 12; ++i) {
		printf("C");
	}
}

void task4(void const *const args) {
	for (int i = 0; i < 12; ++i) {
		printf("D");
	}
}

void task5(void const *const args) {
	for (int i = 0; i < 12; ++i) {
		printf("E");
	}
}

void task6(void const *const args) {
	for (int i = 0; i < 12; ++i) {
		printf("F");
	}
}

/* MAIN FUNCTION */

int main(void) {
	/* Initialise the serial port so printf() works */
	serial_init();

	printf("\r\nDocetOS Demo\r\n");

	/* Reserve memory for two stacks and two TCBs.
	   Remember that stacks must be 8-byte aligned. */
	__align(8)
	static uint32_t stack1[64], stack2[64], stack3[64], stack4[64], stack5[64], stack6[64];
	static OS_TCB_t TCB1, TCB2, TCB3, TCB4, TCB5, TCB6;

	/* Initialise the TCBs using the two functions above */
	OS_initialiseTCB(&TCB1, stack1+64, task1,NULL, 1);//A
	OS_initialiseTCB(&TCB2, stack2+64, task2,NULL, 0);//B
	OS_initialiseTCB(&TCB3, stack3+64, task3,NULL, 1);//C
	OS_initialiseTCB(&TCB4, stack4+64, task4,NULL, 1);//D
	OS_initialiseTCB(&TCB5, stack5+64, task5,NULL, 1);//E
	OS_initialiseTCB(&TCB6, stack6+64, task6,NULL, 2);//F


	/* Initialise and start the OS */
	OS_init(&fixedPriorityScheduler);
	OS_addTask(&TCB1);
	OS_addTask(&TCB2);
	OS_addTask(&TCB3);
	OS_addTask(&TCB4);
	OS_addTask(&TCB5);
//	OS_addTask(&TCB6);
	
	OS_start();
}
