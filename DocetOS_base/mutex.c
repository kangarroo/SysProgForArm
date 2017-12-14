#include "mutex.h"

void OS_initialiseMutex(OS_mutex_t *mutex){
	mutex->counter = 0;
	mutex->task = 0;
	mutex->list.head = 0;
	mutex->list.tail = 0;
}

void OS_mutex_aquire(OS_mutex_t *mutex){
	while(1){
		uint32_t LDREXW_val = __LDREXW(&mutex->task);
		if(LDREXW_val == 0){
			uint32_t STREXW_val = __STREXW((uint32_t)OS_currentTCB(),&mutex->task);
			if(STREXW_val == 0){ //If successful, break loop
				break;
			}
		}else if(LDREXW_val != (uint32_t)OS_currentTCB()){
				OS_wait(&mutex->list);
		}
		
	}
		mutex->counter++;
		OS_yield();
}

void OS_mutex_release(OS_mutex_t *mutex){
	//Safety Check: Checks MUTEX ownership
	OS_TCB_t *current_TCB = OS_currentTCB();
	if(mutex->task == current_TCB){	
		mutex->counter--;
		if(mutex->counter == 0){
			mutex->task = 0;
			OS_notify(&mutex->list);
		}
		OS_yield();
	}
}
