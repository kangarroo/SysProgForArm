#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include "os_internal.h"
#include <stdint.h>
#include "lists.h"

/*Container for the Binary semaphore type*/
typedef struct{
	uint32_t taken;
	priority_list_t queue;
} OS_binarySemaphore_t;

/*Container for the Counitng sempahore type*/
typedef struct{
	uint32_t counter;
	uint32_t counter_max;
	priority_list_t queue;
} OS_countingSemaphore_t;

void OS_binarySemaphore_init(OS_binarySemaphore_t *semaphore);
void OS_countingSemaphore_init(OS_countingSemaphore_t *semaphore, uint32_t numTasks);
void OS_binarySemaphore_take(OS_binarySemaphore_t *semaphore);
void OS_binarySemaphore_release(OS_binarySemaphore_t *semaphore);
void OS_countingSemaphore_wait(OS_countingSemaphore_t *semaphore);
void OS_countingSemaphore_signal(OS_countingSemaphore_t *semaphore);

#endif /*_SEMAPHORE_H_*/
