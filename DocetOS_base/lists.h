#ifndef _LISTS_H_
#define _LISTS_H_

#include "task.h"
#include <stdint.h>

typedef struct {
	OS_TCB_t *head;
	OS_TCB_t *tail;
	uint32_t counter;
} priority_list_t;

typedef struct {
	OS_TCB_t *task;
	uint32_t counter;
	priority_list_t list;
}OS_mutex_t;

void add_task_to_list (priority_list_t *current_list, OS_TCB_t * const tcb);
void remove_task_from_list (priority_list_t *current_list, OS_TCB_t * const tcb);

#endif /*_LISTS_H_*/
