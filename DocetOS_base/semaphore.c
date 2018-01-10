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
}

/*OS Binary Semaphore Take
This function allows */
void OS_binarySemaphore_take(OS_binarySemaphore_t *semaphore){
	while(1){
		uint32_t LDREXW_val = __LDREXW(&semaphore->taken);
		if(LDREXW_val == 0){
			uint32_t STREXW_val = __STREXW(1,&semaphore->taken);
			if(STREXW_val == 0){ //If successful, break loop
				break;
			}
		}else if(LDREXW_val == 1){
				OS_wait(&semaphore->queue);
		}
		
	}
	OS_yield();
}

void OS_binarySemaphore_release(OS_binarySemaphore_t *semaphore){
	while(1){
		uint32_t LDREXW_val = __LDREXW(&semaphore->taken);
		if(LDREXW_val == 1){
				uint32_t STREXW_val = __STREXW(0,&semaphore->taken);
			if(STREXW_val == 0){ //If successful, break loop
				OS_notify(&semaphore->queue);
				break;
			}
		}
	}
	OS_yield();
}

void OS_countingSemaphore_wait(OS_countingSemaphore_t *semaphore){
	while(1){
		uint32_t LDREXW_val = __LDREXW(&semaphore->counter);
		if(LDREXW_val != 0){
			uint32_t STREXW_val = __STREXW(--LDREXW_val,&semaphore->counter);
			if(STREXW_val == 0){
				break;
			}
		} else {
			OS_wait(&semaphore->queue);
		}
	}
}

void OS_countingSemaphore_signal(OS_countingSemaphore_t *semaphore){
	while(1){
		uint32_t LDREXW_val = __LDREXW(&semaphore->counter);
		uint32_t STREXW_val = __STREXW(++LDREXW_val,&semaphore->counter);
		if(STREXW_val == 0){
			break;
		}
	}
	OS_notify(&semaphore->queue);
}
