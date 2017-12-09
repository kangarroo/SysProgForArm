#ifndef _MUTEX_H_
#define _MUTEX_H_

#include "os_internal.h"
#include <stdint.h>


/*Container for the Mutex structure*/
typedef struct {
	OS_TCB_t *task;
	uint32_t counter;
}OS_mutex_t;

void OS_initialiseMutex(OS_mutex_t *mutex);


#endif /*_MUTEX_H_*/