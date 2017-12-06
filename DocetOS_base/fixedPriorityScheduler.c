#include "fixedPriorityScheduler.h"
#include <stdio.h>

typedef struct {
	OS_TCB_t *current_task;
	void *next_task;
} task_list_t;

typedef struct {
	task_list_t *task_list;
	task_list_t *task_list_head;//Pointer to the head of the linked list
	uint32_t init;
	void *next_priority_list;
} priority_list_t;

static void fixedPriority_addTask(OS_TCB_t * const tcb);
static void OS_initialiseScheduler (void);
static void fixedPriority_taskExit(OS_TCB_t * const tcb);
static OS_TCB_t const * fixedPriority_scheduler(void);
static void add_test(task_list_t *current_list, task_list_t *current_head, OS_TCB_t *tcb);

static priority_list_t *high_priority;
static priority_list_t *middle_priority;
static priority_list_t *low_priority;

/*Scheduler Init*/
/*Sets up the initial structure for the priority based scheduler*/
static void OS_initialiseScheduler (void){
	printf("TEST");
	high_priority->task_list->current_task = 0; //Initialises the task list
	high_priority->next_priority_list = middle_priority;//links to next lowest priority level
	high_priority->init = 1;
	middle_priority ->task_list->current_task = 0;
	middle_priority ->next_priority_list = low_priority;
	low_priority ->task_list->current_task = 0;
	low_priority ->next_priority_list = 0;//If no tasks are scheduled, the system should idle.

}

OS_Scheduler_t const fixedPriorityScheduler = {
	.preemptive = 1,
	.scheduler_callback = fixedPriority_scheduler,
	.addtask_callback = fixedPriority_addTask,
	.taskexit_callback = fixedPriority_taskExit
};

/*Add Task Callback*/
/*Adds a task to the scheduler*/
static void fixedPriority_addTask(OS_TCB_t * const tcb){
	if(high_priority->init != 0){ //Checks if the scheduler structure has been initalised
		OS_initialiseScheduler();
	}
	
	if(tcb->priority == 1){ //Add High priority tasks	
		add_test(high_priority->task_list, high_priority->task_list_head, tcb);
	} else if (tcb->priority == 2){ //Add Middle priority tasks
		add_test(middle_priority->task_list, middle_priority->task_list_head, tcb);
	} else { //Add Low Priority tasks
		add_test(low_priority->task_list, low_priority->task_list_head, tcb);
	}
	

}

static void add_test(task_list_t *current_list, task_list_t *current_head, OS_TCB_t *tcb){
	if(current_list->current_task ==  0){
		/* If the list is empty this initialises the linked list as well as 
		the pointer to the head of the linked list.*/
		current_list->current_task = tcb;
		current_list->next_task = 0;
		current_head = current_list;
	}else{
		while(current_list->next_task != 0){
			//Increments list to the first open space
			current_list = current_list->next_task;
		}
		task_list_t *new_task;
		new_task->current_task = tcb;
		new_task->next_task = 0;
		current_list->next_task = new_task;
	}
}

/*Task Exit Callback*/
/*Removes a task from the scheduler once it is completed*/
static void fixedPriority_taskExit(OS_TCB_t * const tcb){
	static task_list_t *current_list; //Container for the task list, that needs tasks added
	static task_list_t *current_head; //Container for the head of the task lists.
	if(tcb->priority == 1){ //Add High priority tasks
		*current_list = *high_priority->task_list;
		*current_head = *high_priority->task_list_head;
	} else if (tcb->priority == 2){ //Add Middle priority tasks
		*current_list = *middle_priority->task_list;
		*current_head = *middle_priority->task_list_head;
	} else { //Add Low Priority tasks
		*current_list = *low_priority->task_list;
		*current_head = *low_priority->task_list_head;
	}
	
	if(current_head->current_task == tcb){
		//If current head is to be removed, replace current head with next in line
		current_head = current_head->next_task;
	} else {
		while(current_list->next_task != tcb){
			//Increments list to the first open space
			current_list = current_list->next_task;
		}
		task_list_t *removed_task;
		//Replaces removed task with next task in list
		removed_task = current_list->next_task;
		current_list->next_task = removed_task->next_task;
	}
}

static OS_TCB_t const * fixedPriority_scheduler(void){
	//Clear yeild flag
	OS_currentTCB()->state &= ~TASK_STATE_YIELD;
	//Check high priority task list
	priority_list_t *current_priority_list = high_priority;
	//Iterate through the list
	while(current_priority_list->next_priority_list != 0){
		task_list_t *current_task_list = current_priority_list->task_list_head;
		while(current_task_list->next_task != 0){
			return current_task_list->current_task;
		}
		current_priority_list = current_priority_list->next_priority_list;
	}
	return OS_idleTCB_p;
}




