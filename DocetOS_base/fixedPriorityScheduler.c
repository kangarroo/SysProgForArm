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
/**/
static void fixedPriority_taskExit(OS_TCB_t * const tcb){
	priority_list_t *current_list = &priority[tcb->priority];
	if(current_list->head == current_list->tail){
		current_list->head = 0;
		current_list->tail = 0;
	} else {
		OS_TCB_t *new_tail = current_list->tail->prev;
		new_tail->next = 0;
		current_list->tail = new_tail; 
	}
}



/*Fixed Priority Scheduler*/

static OS_TCB_t const * fixedPriority_scheduler(void){
	OS_currentTCB()->state &= ~TASK_STATE_YIELD;
	for(int i = 0;i < (MAX_PRIORITY);i++){
		priority_list_t *current_list = &priority[i];
		if(current_list->tail != 0){
			OS_TCB_t *next_task = current_list->tail; 
			fixedPriority_taskExit(next_task);
			fixedPriority_addTask(next_task);
			return current_list->tail;
		}
	}
	return OS_idleTCB_p;
}
