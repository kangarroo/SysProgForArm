#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include "os_internal.h"
#include <stdint.h>
#include "lists.h"

typedef struct{
	uint32_t taken;
	priority_list_t queue;
} OS_binarySemaphore_t;

typedef struct{
	int32_t counter;
	int32_t max_counter;
	priority_list_t queue;
} OS_countingSemaphore_t;



#endif /*_SEMAPHORE_H_*/
