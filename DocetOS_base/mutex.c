#include "mutex.h"

/*Initialise Mutex
Initialises all fields in the Mutex to 0*/
void OS_initialiseMutex(OS_mutex_t *mutex){
	mutex->counter = 0;
	mutex->task = 0;
	mutex->list.head = 0;
	mutex->list.tail = 0;
}

/*OS Mutex Aquire
This function allows a task to aquire the mutex. If the task is successful in
aquireing the mutex, it is set as the task field in the mutex and meaning no other
task can take the mutex until that task frees it. If a task is not successful in
aquiring the mutex, the task is added to the mutex's wait queue using OS_Wait*/
void OS_mutex_aquire(OS_mutex_t *mutex){
	while(1){
		uint32_t LDREXW_val = __LDREXW(&mutex->task);//Load mutex task value
		if(LDREXW_val == 0){ //Check if task is present
			//If there is no task save it to the mutex's task field
			uint32_t STREXW_val = __STREXW((uint32_t)OS_currentTCB(),&mutex->task);
			if(STREXW_val == 0){ //If successful, break loop
				break;
			}
		}else if(LDREXW_val != (uint32_t)OS_currentTCB()){ 
			//if the saved task is not the current task, send task to the mutex's wait queue.
				OS_wait(&mutex->list);
		}
		
	}
		mutex->counter++;
		//OS_yield();
}

/*OS Mutex Release
This releses the task from the mutex, first the mutex task field is compaired to the
current running task, if they match the mutex is freed and OS_notify is run to remove
tasks from the mutex's waiting queue*/

void OS_mutex_release(OS_mutex_t *mutex){
	//Safety Check: Checks MUTEX ownership
	OS_TCB_t *current_TCB = OS_currentTCB();
	if(mutex->task == current_TCB){ //Check if stored task maches current task
		mutex->counter--;
		if(mutex->counter == 0){
			mutex->task = 0;
			OS_notify(&mutex->list);//Free Task from Mutex wait list
		}
		OS_yield();
	}
}




