#ifndef _MUTEX_H_
#define _MUTEX_H_

#include "os_internal.h"
#include <stdint.h>
#include "lists.h"


/*Container for the Mutex structure*/


void OS_initialiseMutex(OS_mutex_t *mutex);
void OS_mutex_aquire(OS_mutex_t *mutex);
void OS_mutex_release(OS_mutex_t *mutex);

#endif /*_MUTEX_H_*/
