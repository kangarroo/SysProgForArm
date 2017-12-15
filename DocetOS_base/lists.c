#include "lists.h"

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
