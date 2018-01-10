#include "semaphore.h"
#include <stdio.h>

/*OS Binary Semaphore Initialise
Initialises all fields in the binary semaphore to 0*/
void OS_binarySemaphore_init(OS_binarySemaphore_t *semaphore){
	semaphore->queue.head = 0;
	semaphore->queue.tail = 0;
	semaphore->taken = 0;
}

/*OS Counting Semaphore Initialise
Initialises all fields in the couning semaphore, and allows the user to
specify how many tokens are avalible to the semaphore*/
void OS_countingSemaphore_init(OS_countingSemaphore_t *semaphore, uint32_t numTasks){
	semaphore->queue.head = 0;
	semaphore->queue.tail = 0;
	semaphore->counter = numTasks;
	semaphore->counter_max = numTasks;
}

/*OS Binary Semaphore Take
This function takes a binary semaphores token and calls
OS wait to manage its waiting task queue. This is achieved
by first checking the current status using LDREXW, if the output
is 0, the semaphore is free to be taken and the value of the token 
is set to 1. Otherwise the task is added to the semaphores wait queue*/
void OS_binarySemaphore_take(OS_binarySemaphore_t *semaphore){
	while(1){
		uint32_t LDREXW_val = __LDREXW(&semaphore->taken);//Check value of taken flag
		if(LDREXW_val == 0){
			uint32_t STREXW_val = __STREXW(1,&semaphore->taken);//Set taken flag and store
			if(STREXW_val == 0){ //If successful, break loop
				break;
			}
		}else if(LDREXW_val == 1){
				OS_wait(&semaphore->queue);//If taken flag active, add TCB to waiting queue
		}
		
	}
}

/*OS Binary Semaphore Release
This function releases a semaphores token for use. This is achieved by
loading the semaphores current status using LDREXW, if the output is 1
the semaphore was taken. The taken value is then set to 0 meaning that
the semaphore is free. OS_notify is then called to release tasks from
the waiting queue.*/
void OS_binarySemaphore_release(OS_binarySemaphore_t *semaphore){
	while(1){
		uint32_t LDREXW_val = __LDREXW(&semaphore->taken);//Check if taken flag is active
		if(LDREXW_val == 1){
				uint32_t STREXW_val = __STREXW(0,&semaphore->taken);//Reset taken flag and store
			if(STREXW_val == 0){ //If successful, break loop
				OS_notify(&semaphore->queue);//Free task from waiting queue
				break;
			}
		}
	}
	OS_yield();
}

/*OS Counting Semaphore Wait
This function assigns semaphore tokens to tasks that request them.
First the current number of remaining tokens is loaded, if this value is
not 0, the value is decremented and the task is run. When the number of
tokens reaches 0, OS_Wait is run to add the task to the waiting queue.*/
void OS_countingSemaphore_wait(OS_countingSemaphore_t *semaphore){
	while(1){
		uint32_t LDREXW_val = __LDREXW(&semaphore->counter);//Loads number of remaining tokens
		if(LDREXW_val != 0){
			uint32_t STREXW_val = __STREXW(--LDREXW_val,&semaphore->counter);//Decrement number of tokens and store
			if(STREXW_val == 0){
				break;
			}
		} else {
			OS_wait(&semaphore->queue);//If no tokens remain, add task to waiting list
		}
	}
}

/*OS Counting Semaphore Signal
This function releases semaphore tokens from tasks. First the current number
of semaphore tokens is loaded, if this value is less than the semaphores max
value, the value is incremented and OS notify is run to release tasks from the
semaphores wait queue.*/
void OS_countingSemaphore_signal(OS_countingSemaphore_t *semaphore){
	while(1){
		uint32_t LDREXW_val = __LDREXW(&semaphore->counter);//Load number of remaining tokens
		if(LDREXW_val != semaphore->counter_max){
			uint32_t STREXW_val = __STREXW(++LDREXW_val,&semaphore->counter);//Increment value of tokens and store
			if(STREXW_val == 0){
				break;
			}
		}
	}
	OS_notify(&semaphore->queue);//Release tasks from the semaphores wait queue.
}
