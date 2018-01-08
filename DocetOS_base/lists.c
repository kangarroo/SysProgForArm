#include "lists.h"
#include "os_internal.h"
#include <stdio.h>
void add_task_to_list (priority_list_t *current_list, OS_TCB_t * const tcb){
	
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
	//Iterate the list counter
	current_list->counter++;
}

void add_task_to_sleep_list (priority_list_t *current_list,OS_TCB_t * const tcb){
	//printf("TEST\r\n");
	
	if(current_list->head == 0){
		/*If the list is empty, set the head and tail to the new TCB address
		and set TCB Next and Previous pointers to 0*/
		tcb->next = 0;
		tcb->prev = 0;
		current_list->head = tcb;
		current_list->tail = tcb;
	} else {
		OS_TCB_t *current_task = current_list->tail;
		while(current_task->prev != 0){
			//Compare the wake times of the task to add and those already in the list
		  //if the sleep time is lower than the current one being checked, add new
			//task before it.
			if(tcb->sleep_time<=current_task->sleep_time){
				if(current_task == current_list->tail){
					//If the current task is the tail
					tcb->next =0;
					tcb->prev = current_list->tail;
					current_list->tail = tcb;
					return;
				} else {
					//If the current task is between other tasks add the task infront of
					//the current task
					tcb->next = current_task;
					tcb->prev = current_task->prev;
					current_task->prev = tcb;
					current_task->prev->next = tcb;
					
					return;
				}
			}
			current_task = current_task->prev;
		}
		
		/*If the tasks waiting time is the slowest, make it the head of the list*/
		OS_TCB_t *current_head = current_list->head;
		current_head->prev = tcb;
		tcb->next = current_head;
		tcb->prev = 0;
		current_list->head = tcb;
	}
		/*If there is already data in the structure, set new tcb as head poinitng*/
		current_list->counter++;
		//printf("List size: %d", current_list->counter);
	}
	//Iterate the list counter
	



void remove_task_from_list (priority_list_t *current_list, OS_TCB_t * const tcb){
	OS_TCB_t *current_task = current_list->tail;
	
	if(current_list->head == current_list->tail){
		//If the list only has one object, the head and tail pointers should be set
		//to 0, it should be noted that this doesn't free the memory used by the system...
		if(current_list->tail == tcb){
			current_list->head = 0;
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
	//Decrement the list counter
	current_list->counter--;
}
