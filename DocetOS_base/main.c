#include "os.h"
#include <stdio.h>
#include "utils/serial.h"
#include "sleep.h"
#include "fixedPriorityScheduler.h"
#include "stm32f4xx.h"
#include "mutex.h"
#include "semaphore.h"
#include "demo.h"

/* MAIN FUNCTION */
int main(void) {
	/* Initialise the serial port so printf() works */
	serial_init();

	printf("\r\n\r\nDocetOS Demo\r\n");
	printf("Y3604419");

	/* Reserve memory for two stacks and two TCBs.
	   Remember that stacks must be 8-byte aligned. */
	__align(8)
	//Stacks for the demo tasks
	static uint32_t stack1[64],stack2[64],stack3[64],stack4[64],stack5[64],stack6[64],
									stack7[64],stack8[64],stack9[64],stack10[64];
	//Demo task TCBs
	static OS_TCB_t scheduler_demo_TCB, mutex_demo_TCB, sleep_demo_TCB
		,binSem_demo_TCB,countSem_demo_TCB,end_TCB,scheduler_intro_TCB,
		mutex_intro_TCB, sleep_intro_TCB, semaphore_intro_TCB;
	
	/*Initialise the demo binary semaphore*/	
	/*This semaphore is needed to syncronise timings of
	the OS demo*/
	OS_binarySemaphore_init(&DEMO_SEMAPHORE);
	
	/* Initialise the TCBs using the two functions above */
	
	OS_initialiseTCB(&scheduler_demo_TCB, stack1+64, scheduler_demo,NULL, 0);
	OS_initialiseTCB(&mutex_demo_TCB, stack2+64, mutex_demo,NULL,0);
	OS_initialiseTCB(&sleep_demo_TCB, stack3+64, sleep_demo,NULL,0);
	OS_initialiseTCB(&binSem_demo_TCB,stack4+64, binSem_demo,NULL,0);
	OS_initialiseTCB(&countSem_demo_TCB,stack5+64, countSem_demo,NULL,0);
	OS_initialiseTCB(&end_TCB,stack6+64,demo_end,NULL,0);
	OS_initialiseTCB(&scheduler_intro_TCB, stack7+64, scheduler_intro, NULL, 0);
	OS_initialiseTCB(&mutex_intro_TCB, stack8+64, mutex_intro, NULL,0);
	OS_initialiseTCB(&sleep_intro_TCB, stack9+64, sleep_intro, NULL, 0);
	OS_initialiseTCB(&semaphore_intro_TCB, stack10+64, semaphore_intro, NULL,0);
	
	
	/* Initialise and start the OS */
	OS_init(&fixedPriorityScheduler);
	
	/*Add demo to scheduler*/
	OS_addTask(&scheduler_intro_TCB);
	OS_addTask(&scheduler_demo_TCB);
	OS_addTask(&mutex_intro_TCB);
	OS_addTask(&mutex_demo_TCB);
	OS_addTask(&sleep_intro_TCB);
	OS_addTask(&sleep_demo_TCB);
	OS_addTask(&semaphore_intro_TCB);
	OS_addTask(&binSem_demo_TCB);
	OS_addTask(&countSem_demo_TCB);
	OS_addTask(&end_TCB);
	
	OS_start();
}
