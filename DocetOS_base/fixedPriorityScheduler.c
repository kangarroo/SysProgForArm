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
	OS_TCB_t *current_task;
	void *next_task;
	void *prev_task;
} task_list_t;

/*Priority List*/
/*Structure to hold the Task Lists for a given priority level*/
typedef struct {
	task_list_t *head;
	task_list_t *tail;
} priority_list_t;

/*Scheduler Type*/
/*General container for the scheduler priority lists*/
typedef struct {
	priority_list_t *high;
	priority_list_t *middle;
	priority_list_t *low;
	uint32_t init;
} scheduler_t;

static void fixedPriority_addTask(OS_TCB_t * const tcb);
static void OS_initialiseScheduler (void);
static void fixedPriority_taskExit(OS_TCB_t * const tcb);
static OS_TCB_t const * fixedPriority_scheduler(void);
static void add_task (priority_list_t *priority, OS_TCB_t * const tcb);
static void remove_task(priority_list_t *priority, OS_TCB_t * const tcb);

static scheduler_t fP_Scheduler;


/*Scheduler Init*/
/*Initialises the values of the heads and tails for every priority level*/
static void OS_initialiseScheduler (void){
	//Initialise the Heads and tails
	fP_Scheduler.high->head = 0;
	fP_Scheduler.high->tail = 0;
	fP_Scheduler.middle->head = 0;
	fP_Scheduler.middle->tail = 0;
	fP_Scheduler.low->head = 0;
	fP_Scheduler.low->tail = 0;
	fP_Scheduler.init = 1;
}

OS_Scheduler_t const fixedPriorityScheduler = {
	.preemptive = 1,
	.scheduler_callback = fixedPriority_scheduler,
	.addtask_callback = fixedPriority_addTask,
	.taskexit_callback = fixedPriority_taskExit
};

/*Add Task Callback*/
/*Enqueues a task on to the relevant queue as dictated by the TCB priority value*/
static void fixedPriority_addTask(OS_TCB_t * const tcb){
	if(fP_Scheduler.init != 1){
		OS_initialiseScheduler();
	}
		
	if(tcb->priority == 1){
		add_task(fP_Scheduler.high, tcb);
	} else if (tcb->priority == 2){
		add_task(fP_Scheduler.middle, tcb);
	} else {
		add_task(fP_Scheduler.low, tcb);
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
		new_task.next_task = &priority->head;
		new_task.prev_task = 0;
		priority->head = &new_task;
	}
	
}


/*Task Exit Callback*/
/*Removes a task from the scheduler once it is completed*/
static void fixedPriority_taskExit(OS_TCB_t * const tcb){
	if(tcb->priority == 1){
		remove_task(fP_Scheduler.high, tcb);
	} else if (tcb->priority == 2){
		remove_task(fP_Scheduler.middle, tcb);
	} else {
		remove_task(fP_Scheduler.low, tcb);
	}
}

static void remove_task(priority_list_t *priority, OS_TCB_t * const tcb){
	if(priority->head == priority->tail){
		priority->head = 0;
		priority->tail = 0;
	} else {
		task_list_t *new_tail = priority->tail->prev_task;
		new_tail->next_task = 0;
		priority->tail = new_tail;
	}
}

static OS_TCB_t const * fixedPriority_scheduler(void){
	OS_currentTCB()->state &= ~TASK_STATE_YIELD;
	//Check for HIGH Priority Tasks
	while(&(fP_Scheduler.high->head) != 0){
		return fP_Scheduler.high->tail->current_task;
	}
	while(&(fP_Scheduler.middle->head) != 0){
		return fP_Scheduler.middle->tail->current_task;
	}
	while(&(fP_Scheduler.low->head) != 0){
		return fP_Scheduler.low->tail->current_task;
	}
	return OS_idleTCB_p;
}




