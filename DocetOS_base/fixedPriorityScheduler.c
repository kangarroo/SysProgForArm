#include "fixedPriorityScheduler.h"
#include <stdio.h>
/*Fixed Priority Scheduler*/
/*This Scheduler is based on 3 double linked list queues, linked together
to make 3 different priorities High, Middle, Low. These lists are chosen by
the priority value in the TCB which is a value from 1-3 with 1 being the highest
priority*/

/*Task List*/
/*Double linked list to hold the tasks for a particular priority level*/
typedef struct {
	OS_TCB_t * current_task;
	void *next_task;
	void *prev_task;
} task_list_t;

/*Priority List*/
/*Structure to hold the Task Lists for a given priority level*/
typedef struct {
	task_list_t *head;
	task_list_t *tail;
} priority_list_t;

static void fixedPriority_addTask(OS_TCB_t * const tcb);
static void fixedPriority_taskExit(OS_TCB_t * const tcb);
static OS_TCB_t const * fixedPriority_scheduler(void);
static void add_task (priority_list_t *priority, OS_TCB_t * const tcb);
static void remove_task(priority_list_t *priority, OS_TCB_t * const tcb);

//static scheduler_t fP_Scheduler;
static priority_list_t high = {
	.head = 0,
	.tail = 0
};
static priority_list_t middle = {
	.head = 0,
	.tail = 0
};
static priority_list_t low = {
	.head = 0,
	.tail = 0
};

OS_Scheduler_t const fixedPriorityScheduler = {
	.preemptive = 1,
	.scheduler_callback = fixedPriority_scheduler,
	.addtask_callback = fixedPriority_addTask,
	.taskexit_callback = fixedPriority_taskExit
};

/*Add Task Callback*/
/*Enqueues a task on to the relevant queue as dictated by the TCB priority value*/
static void fixedPriority_addTask(OS_TCB_t * const tcb){
//	if(fP_Scheduler.init != 1){
//		OS_initialiseScheduler(&fP_Scheduler);
//	}
		
	if(tcb->priority == 1){
		//printf("HIGH\r\n");
		add_task(&high, tcb);
	} else if (tcb->priority == 2){
		//printf("MIDDLE\r\n");
		add_task(&middle, tcb);
	} else {
		//printf("LOW\r\n");
		add_task(&low, tcb);
	}
}

/*Adds tasks to the head of the queue*/
static void add_task (priority_list_t *priority, OS_TCB_t * const tcb){
	task_list_t new_task;
	if(priority->head == 0){
		new_task.current_task = tcb;
		new_task.next_task = 0;
		new_task.prev_task = 0;
		priority->head = &new_task;
		priority->tail = &new_task;
	} else {
		new_task.current_task = tcb;
		priority->head->prev_task = &new_task;
		new_task.next_task = (task_list_t*)&priority->head;
		new_task.prev_task = 0;
		priority->head = &new_task;
	}
	
}


/*Task Exit Callback*/
/*Removes a task from the scheduler once it is completed*/
static void fixedPriority_taskExit(OS_TCB_t * const tcb){
	if(tcb->priority == 1){
		remove_task(&high, tcb);
	} else if (tcb->priority == 2){
		remove_task(&middle, tcb);
	} else {
		remove_task(&low, tcb);
	}
}

static void remove_task(priority_list_t *priority, OS_TCB_t * const tcb){
	if(priority->head == priority->tail){
		priority->head = 0;
		priority->tail = 0;
	} else {
		task_list_t *new_tail = (task_list_t*)priority->tail->prev_task;
		new_tail->next_task = 0;
		priority->tail = new_tail;
	}
}

static OS_TCB_t const * fixedPriority_scheduler(void){
	OS_currentTCB()->state &= ~TASK_STATE_YIELD;
	//Check for HIGH Priority Tasks
	while((high.head) != 0){
		return high.tail->current_task;
	}
	while((middle.head) != 0){
		return middle.tail->current_task;
	}
	while((low.head) != 0){
		return low.tail->current_task;
	}
	return OS_idleTCB_p;
}




