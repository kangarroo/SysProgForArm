#include "fixedPriorityScheduler.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include "lists.h"

/*Fixed Priority Scheduler*/

static void fixedPriority_addTask(OS_TCB_t * const tcb);
static void fixedPriority_taskExit(OS_TCB_t * const tcb);
static OS_TCB_t const * fixedPriority_scheduler(void);
static void preemptive_tasks (OS_TCB_t * const tcb);
static void fixedPriority_wait(priority_list_t * const waitList, uint32_t check_code);
static void fixedPriority_notify(priority_list_t * const waitList);
static OS_TCB_t* fixedPriority_taskSleep (OS_TCB_t * const sleeping_task);
static void fixedPriority_checkSleepList(void);

priority_list_t priority[MAX_PRIORITY] = {0};//Array of task lists
priority_list_t sleep_list = {0};//List of sleeping tasks

/*Schedular callback structure
This links the schedular functions to the scheduler function
callbacks.*/
OS_Scheduler_t const fixedPriorityScheduler = {
	.preemptive = 1,
	.scheduler_callback = fixedPriority_scheduler,
	.addtask_callback = fixedPriority_addTask,
	.taskexit_callback = fixedPriority_taskExit,
	.wait_callback = fixedPriority_wait,
	.notify_callback = fixedPriority_notify
};

/*Add Task Callback*/
/*This adds a task to the relevant priority task list. This is
set using the tasks internal priorty value where 0 is highest priority.*/
static void fixedPriority_addTask(OS_TCB_t * const tcb){
		priority_list_t *current_list = &priority[tcb->priority];//Select correct task list
		add_task_to_list(current_list,tcb);
}



/*Task Exit Callback*/
/*Removes a task from the task list it is assigned to, this is set using the
tasks internal priority value*/
static void fixedPriority_taskExit(OS_TCB_t * const tcb){
	priority_list_t *current_list = &priority[tcb->priority];
	remove_task_from_list(current_list,tcb);
}



/*Fixed Priority Scheduler
This task maintains the schedular task lists.*/

static OS_TCB_t const * fixedPriority_scheduler(void){
	OS_currentTCB()->state &= ~TASK_STATE_YIELD; //Clear the task yield state

	for(int i = 0;i < (MAX_PRIORITY);i++){
		/*Check sleepList to see if the tail has woken up, if so add the task back to
		the schedulars list.*/
		fixedPriority_checkSleepList(); 
		/*Select correct list for the current task, this is set using the tasks internal
		priority value.*/
		priority_list_t *current_list = &priority[i];
		if(current_list->tail != 0){
			OS_TCB_t *next_task = current_list->tail;
			/*To allow the system to pre-emptivly switch tasks, the current task is dequeued and then 
			re-enqeued, if this isn't done running a task switch would effectivly run the same task*/
			preemptive_tasks(next_task);				
			/*Check if the current task's sleep flag is active*/
			if(next_task->state & TASK_STATE_SLEEP){
				/*If current task is sleeping, remove from list and add to sleep list
				the task sleep task also returns the current tasks previous value, so no extra
				cycle is needed to run the next task*/
				OS_TCB_t *task = fixedPriority_taskSleep(next_task);
				if(task != 0){ //check if task is not 0
					return task;
				}
			} else {
				return next_task;
			}
		}
	}
	/*If the current task is 0, poll the idle task till a new task is added to the list*/
	return OS_idleTCB_p;
}

/*Fixed Priority task sleep
This function removes tasks from the scheduler list and adds them to the sleep list,
it also returns the pointer to the next task to be run (the task previous to the sleeping task)*/
static OS_TCB_t* fixedPriority_taskSleep (OS_TCB_t * const sleeping_task){
	OS_TCB_t *task_to_run = sleeping_task->prev;
	fixedPriority_taskExit(sleeping_task);
	sleeping_task->next = 0;
	sleeping_task->prev = 0;
	add_task_to_sleep_list(&sleep_list,sleeping_task);
	return task_to_run;
}
/*Fixed Priority check sleep list
This function checks if the tail of the sleep list has woken up (since the sleep list is time sorted),
if the task is no longer sleeping, the task is removed of the sleep list, has its sleep flag cleared and
is added to the scheduler lst.*/
static void fixedPriority_checkSleepList(void){
	if(sleep_list.tail != 0 && sleep_list.tail->wake_time <= OS_elapsedTicks()){
		OS_TCB_t *task_temp = sleep_list.tail;
		task_temp->state &= ~TASK_STATE_SLEEP;
		remove_task_from_list(&sleep_list, task_temp);
		fixedPriority_addTask(task_temp);
	}
}

/*Preemptive Tasks*/
/*This Moves the tail of the list to the head to allow the 
scheduler to be preemptive*/
static void preemptive_tasks (OS_TCB_t * const tcb){
	fixedPriority_taskExit(tcb);
	fixedPriority_addTask(tcb);
}

/*Wait Callback*/
/*This is called when a MUTEX has already been taken, or if a semaphore is alredy full.
It removes the task from the schedular list and adds it to the mutex's/semaphore's internal queue,
as an additional check is also compaires the check codes to ensure race conditions don't occure*/
static void fixedPriority_wait(priority_list_t * const waitList, uint32_t check_code){
	OS_TCB_t *current_TCB = OS_currentTCB();
	if(check_code == OS_checkCode()){
		fixedPriority_taskExit(current_TCB);
		add_task_to_list(waitList,current_TCB);
	}
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/*Notify Callback*/
/*This function removes tasks from the wait list once the mutex/semaphore on
them is freed, they are then added to the scheduler. The function is 
called once a mutex/sempahore token has been released*/
static void fixedPriority_notify(priority_list_t * const waitList){	
	OS_TCB_t *task_temp = waitList->tail;
	remove_task_from_list(waitList, task_temp);
	fixedPriority_addTask(task_temp);
}
