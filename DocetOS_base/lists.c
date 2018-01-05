#include "lists.h"
#include "os_internal.h"
#include <stdio.h>
void add_task_to_list (priority_list_t *current_list, OS_TCB_t * const tcb){
	current_list->counter++;
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

void add_task_to_sleep_list (priority_list_t *current_list,OS_TCB_t * const tcb){
	current_list->counter++;
	uint32_t tcb_sleep = 0;
	uint32_t task_sleep = 0;
	if(current_list->head == 0){
		//printf("4\r\n");
		/*If the list is empty, set the head and tail to the new TCB address
		and set TCB Next and Previous pointers to 0*/
		tcb->next = 0;
		tcb->prev = 0;
		current_list->head = tcb;
		current_list->tail = tcb;
	} else {
		OS_TCB_t *current_task = current_list->head;
		while(current_task->next != 0){
			tcb_sleep = tcb->sleep_time - OS_elapsedTicks();
			task_sleep = current_task->sleep_time;
			printf("Current Sleep Time: %d, TCB Sleep Time %d\r\n",task_sleep,tcb_sleep);
			if(tcb_sleep <= task_sleep){
				//printf("1\r\n");
				tcb->prev = current_task;
				tcb->next = current_task->next;
				current_task->next->prev = tcb;
				current_task->next = tcb;
				if(tcb->prev == 0){
					current_list->head = tcb;
				}
				return;
			}
			//printf("2\r\n");
			current_task = current_task->next;
		}
		//printf("3\r\n");
		//If the task has the lowest time, it becomes the tail
		OS_TCB_t *tail = current_list->tail;
		tcb->next = 0;
		tcb->prev = tail;
		tail->next = tcb;
		current_list->tail = tcb;
	}
}

void remove_task_from_list (priority_list_t *current_list, OS_TCB_t * const tcb){
	current_list->counter--;
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
}
