#ifndef _LISTS_H_
#define _LISTS_H_

#include "task.h"
#include <stdint.h>

/*Generic structure for a task list
This is the data type for all task lists used in DocetOS*/
typedef struct {
	OS_TCB_t *head;
	OS_TCB_t *tail;
	uint32_t counter;
} priority_list_t;

void add_task_to_list (priority_list_t *current_list, OS_TCB_t * const tcb);
void remove_task_from_list (priority_list_t *current_list, OS_TCB_t * const tcb);
void add_task_to_sleep_list (priority_list_t *current_list,OS_TCB_t * const tcb);

#endif /*_LISTS_H_*/
