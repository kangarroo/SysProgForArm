#include "fixedPriorityScheduler.h"
#include <stdio.h>
#include "stm32f4xx.h"
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
static void fixedPriority_wait(void * const reason);
static void fixedPriority_notify(void * const reason);
static void add_task_to_list (priority_list_t *current_list, OS_TCB_t * const tcb);
static void remove_task_from_list (priority_list_t *current_list, OS_TCB_t * const tcb);

priority_list_t priority[MAX_PRIORITY] = {0};
priority_list_t wait_list = {0};

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

static void add_task_to_list (priority_list_t *current_list, OS_TCB_t * const tcb){
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
/*Note I think the issue is still that the */
static void fixedPriority_taskExit(OS_TCB_t * const tcb){
	priority_list_t *current_list = &priority[tcb->priority];
	remove_task_from_list(current_list,tcb);
}

static void remove_task_from_list (priority_list_t *current_list, OS_TCB_t * const tcb){
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
					} else if(current_task->prev == 0){
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
			preemptive_tasks(next_task);	
			//Checks if sleep bit is set
			if(next_task->state & TASK_STATE_SLEEP){
				if(next_task->sleep_time <= OS_elapsedTicks()){
					//If set & time exceeded, clear sleep bit and return TCB, otherwise
					//return idle tcb
					next_task->state &= ~TASK_STATE_SLEEP;
					return next_task;
				}
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
static void fixedPriority_wait(void * const reason){
	OS_TCB_t *current_TCB = OS_currentTCB();
	current_TCB->data = (uint32_t) reason;
	fixedPriority_taskExit(current_TCB);
	add_task_to_list(&wait_list,current_TCB);
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/*Notify Callback*/
/*This function removes tasks from the wait list once the mutex on
them is freed, they are then added to the scheduler. The function is 
called once a mutex has been released*/
static void fixedPriority_notify(void * const reason){
	priority_list_t *current_list = &wait_list;
	OS_TCB_t *current_task = current_list->tail;
	while(current_task != 0){
		if(current_task->data == (uint32_t)reason){
			current_task->data = 0;
			remove_task_from_list(&wait_list,current_task);
			fixedPriority_addTask(current_task);
			return;
		}
		current_task = current_task->prev;
	}
}
