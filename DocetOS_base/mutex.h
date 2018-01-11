#ifndef _MUTEX_H_
#define _MUTEX_H_

#include "stm32f4xx.h"
#include "os.h"
#include <stdint.h>
#include "lists.h"


/*Container for the Mutex structure*/
typedef struct {
	OS_TCB_t *task;
	uint32_t counter;
	priority_list_t list;
}OS_mutex_t;

void OS_initialiseMutex(OS_mutex_t *mutex);
void OS_mutex_aquire(OS_mutex_t *mutex);
void OS_mutex_release(OS_mutex_t *mutex);

#endif /*_MUTEX_H_*/
