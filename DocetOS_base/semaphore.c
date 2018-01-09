#include "semaphore.h"

void OS_binarySemaphore_init(OS_binarySemaphore_t *semaphore){
	semaphore->queue.head = 0;
	semaphore->queue.tail = 0;
	semaphore->taken = 0;
}

void OS_countingSemaphore_init(OS_countingSemaphore_t *semaphore, int32_t numTasks){
	semaphore->queue.head = 0;
	semaphore->queue.tail = 0;
	semaphore->counter = numTasks;
	semaphore->max_counter = numTasks;
}

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
			int32_t new_counter_val = semaphore->counter--;
			uint32_t STREXW_val = __STREXW(new_counter_val,&semaphore->counter);
			if(STREXW_val == 0){ //If successful, break loop
				if(new_counter_val < 0){
					OS_wait(&semaphore->queue);
				}
				break;
			}
		}
	}
	OS_yield();
}

void OS_countingSemaphore_signal(OS_countingSemaphore_t *semaphore){
	while(1){
		uint32_t LDREXW_val = __LDREXW(&semaphore->counter);
		int32_t new_counter_val = semaphore->counter++;
		uint32_t STREXW_val = __STREXW(new_counter_val,&semaphore->counter);
		if(STREXW_val == 0){
			if(new_counter_val < 0){
				OS_notify(&semaphore->queue);
			}
			break;
		}
	}
}
