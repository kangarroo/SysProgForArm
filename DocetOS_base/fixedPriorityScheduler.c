#include "fixedPriorityScheduler.h"
#include <stdio.h>
/*Fixed Priority Scheduler*/
/*The scheduler works by having an array of linked lists which all represent a priority
which is stored in the TCB structure. The TCB structure also stores the next and previous
addresses for the linked lists*/

/*Priority List*/
//Container for the Priority lists
typedef struct {
	OS_TCB_t *head;
	OS_TCB_t *tail;
} priority_list_t;

static void fixedPriority_addTask(OS_TCB_t * const tcb);
static void fixedPriority_taskExit(OS_TCB_t * const tcb);
static OS_TCB_t const * fixedPriority_scheduler(void);
static void preemptive_tasks (OS_TCB_t * const tcb);


priority_list_t priority[MAX_PRIORITY] = {0};

OS_Scheduler_t const fixedPriorityScheduler = {
	.preemptive = 1,
	.scheduler_callback = fixedPriority_scheduler,
	.addtask_callback = fixedPriority_addTask,
	.taskexit_callback = fixedPriority_taskExit
};

/*Add Task Callback*/
/*Adds a TCB to the scheduler with respect to the priority level set in the TCB*/
static void fixedPriority_addTask(OS_TCB_t * const tcb){
		priority_list_t *current_list = &priority[tcb->priority];
		if(current_list->head == 0){
			/*If the list is empty, set the head and tail to the new TCB address
			and set TCB Next and Previous pointers to 0*/
			tcb->next = 0;
			tcb->prev = 0;
			current_list->head = tcb;
			current_list->tail = tcb;
		} else {
			/*If there is already data in the structure, set new tcb as head poinitng
			to the old head as previous*/
			OS_TCB_t *current_head = current_list->head;
			current_head->prev = tcb;
			tcb->next = current_head;
			tcb->prev = 0;
			current_list->head = tcb;
		}
}

/*Task Exit Callback*/
/*This function removes a task from the scheduler this is achieved through
if the list only has 1 item setting the head and tail of the list to 0, and otherwise
setting the previous element in the list as the tail*/
static void fixedPriority_taskExit(OS_TCB_t * const tcb){
	priority_list_t *current_list = &priority[tcb->priority];
	OS_TCB_t *current_task = current_list->tail;
	
	if(current_list->head == current_list->tail){
		//If the list only has one object, the head and tail pointers should be set
		//to 0, it should be noted that this doesn't free the memory used by the system...
		if(current_list->tail == tcb){
			current_list->head = 0; //
			current_list->tail = 0;
		}
	} else {
			while(current_task != 0){
				if(current_task == tcb){
					if(current_task == current_list->tail){
						//If the current element is the tail, the set the previous element as the tail
						current_list->tail = current_list->tail->prev;
						current_list->tail->next = 0;
					} if(current_task->prev == 0){
						//Remove TCB from head of list
						current_list->head = current_list->head->next;
						current_list->head->prev = 0;
					} else {
						//If the current element is in the middle of the list, the next and previous
						//of the surrounding elements need to be updated

						current_task->prev->next = current_task->next;
						current_task->next->prev = current_task->prev;
					}
					return;
				} else { //Iterates through the list to find the element to be removed
					current_task = current_task->prev;
				}
			}
	}
}



/*Fixed Priority Scheduler*/
/*This function works by iterating through the prioirty array*/
static OS_TCB_t const * fixedPriority_scheduler(void){
	OS_currentTCB()->state &= ~TASK_STATE_YIELD; //Clear the task yield state
	for(int i = 0;i < (MAX_PRIORITY);i++){
		priority_list_t *current_list = &priority[i];
		if(current_list->tail != 0){
			OS_TCB_t *next_task = current_list->tail;
			//preemptive_tasks(next_task);
				fixedPriority_taskExit(next_task);
				fixedPriority_addTask(next_task);
			//Checks if sleep bit is set
			if(current_list->tail->state & TASK_STATE_SLEEP){
				if(current_list->tail->sleep_time <= OS_elapsedTicks()){
					//If set & time exceeded, clear sleep bit and return TCB, otherwise
					//return idle tcb
					current_list->tail->state &= ~TASK_STATE_SLEEP;
					return next_task;
				}
			} else {
				return next_task;
			}
		}
	}
	return OS_idleTCB_p;
}

static void preemptive_tasks (OS_TCB_t * const tcb){
	fixedPriority_taskExit(tcb);
	fixedPriority_addTask(tcb);
}


