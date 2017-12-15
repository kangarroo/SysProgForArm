#include "fixedPriorityScheduler.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include "lists.h"
/*Fixed Priority Scheduler*/
/*The scheduler works by having an array of linked lists which all represent a priority
which is stored in the TCB structure. The TCB structure also stores the next and previous
addresses for the linked lists*/

/*Priority List*/
//Container for the Priority lists


static void fixedPriority_addTask(OS_TCB_t * const tcb);
static void fixedPriority_taskExit(OS_TCB_t * const tcb);
static OS_TCB_t const * fixedPriority_scheduler(void);
static void preemptive_tasks (OS_TCB_t * const tcb);
static void fixedPriority_wait(priority_list_t * const reason);
static void fixedPriority_notify(priority_list_t * const reason);


priority_list_t priority[MAX_PRIORITY] = {0};
priority_list_t sleep_list = {0};

OS_Scheduler_t const fixedPriorityScheduler = {
	.preemptive = 1,
	.scheduler_callback = fixedPriority_scheduler,
	.addtask_callback = fixedPriority_addTask,
	.taskexit_callback = fixedPriority_taskExit,
	.wait_callback = fixedPriority_wait,
	.notify_callback = fixedPriority_notify
};

/*Add Task Callback*/
/*Adds a TCB to the scheduler with respect to the priority level set in the TCB*/
static void fixedPriority_addTask(OS_TCB_t * const tcb){
		priority_list_t *current_list = &priority[tcb->priority];
		add_task_to_list(current_list,tcb);
}



/*Task Exit Callback*/
/*This function removes a task from the scheduler this is achieved through
if the list only has 1 item setting the head and tail of the list to 0, and otherwise
setting the previous element in the list as the tail*/
/*Note I think the issue is still that the */
static void fixedPriority_taskExit(OS_TCB_t * const tcb){
	priority_list_t *current_list = &priority[tcb->priority];
	remove_task_from_list(current_list,tcb);
}



/*Fixed Priority Scheduler*/
/*This function works by iterating through the prioirty array*/
static OS_TCB_t const * fixedPriority_scheduler(void){
	OS_currentTCB()->state &= ~TASK_STATE_YIELD; //Clear the task yield state
	for(int i = 0;i < (MAX_PRIORITY);i++){
		if(sleep_list.tail->sleep_time <= OS_elapsedTicks()){
			OS_TCB_t *finished_sleeping = sleep_list.tail;
			remove_task_from_list(&sleep_list,finished_sleeping);
		  OS_TCB_t *old_tail = priority[finished_sleeping->priority].tail;
			priority[finished_sleeping->priority].tail->next = finished_sleeping;
			finished_sleeping->prev = old_tail;
			priority[finished_sleeping->priority].tail = finished_sleeping;
			/*!!!!!!FIX SLEEPING!!!!!!*/
			//fixedPriority_addTask(finished_sleeping);	
		}
		priority_list_t *current_list = &priority[i];
		if(current_list->tail != 0){
			OS_TCB_t *next_task = current_list->tail;
			preemptive_tasks(next_task);	
			//Checks if sleep bit is set
			if(next_task->state & TASK_STATE_SLEEP){
				//if(next_task->sleep_time <= OS_elapsedTicks()){
					//If set & time exceeded, clear sleep bit and return TCB, otherwise
					//return idle tcb
					next_task->state &= ~TASK_STATE_SLEEP;
					add_task_to_list(&sleep_list,next_task);
					fixedPriority_taskExit(next_task);
					//return next_task;
				//}
			} else {
				return next_task;
			}
		}
	}
	return OS_idleTCB_p;
}
/*Preemptive Tasks*/
/*This Moves the tail of the list to the head to allow the 
scheduler to be preemptive*/
static void preemptive_tasks (OS_TCB_t * const tcb){
	fixedPriority_taskExit(tcb);
	fixedPriority_addTask(tcb);
}

/*Wait Callback*/
/*This function sets the reason code as the data field in the TCB
and then removes it form the schedule, and adds it till the wait list.
The function is called upon creation of a mutex*/
static void fixedPriority_wait(priority_list_t * const reason){
	OS_TCB_t *current_TCB = OS_currentTCB();
	//current_TCB->data = (uint32_t) reason;
	fixedPriority_taskExit(current_TCB);
	//remove_task_from_list(&priority[current_TCB->priority],current_TCB);
	//current_TCB->next = 0;
	//current_TCB->prev = 0;
	add_task_to_list(reason,current_TCB);
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/*Notify Callback*/
/*This function removes tasks from the wait list once the mutex on
them is freed, they are then added to the scheduler. The function is 
called once a mutex has been released*/
static void fixedPriority_notify(priority_list_t * const reason){
	OS_TCB_t *test = reason->tail;
	remove_task_from_list(reason, test);
	fixedPriority_addTask(test);
  
}
