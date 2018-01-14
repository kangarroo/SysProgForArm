#ifndef _DEMO_H_
#define _DEMO_H_

#include "semaphore.h"
#include "mutex.h"

static OS_binarySemaphore_t DEMO_SEMAPHORE; //To ensure that only 1 demo runs at a time
static OS_mutex_t mutex; //For use in the Mutex Demo
static OS_binarySemaphore_t bin_sem;
static OS_countingSemaphore_t count_sem;

void scheduler_demo(void const *const args);
void mutex_demo(void const *const args);
void sleep_demo(void const *const args);
void binSem_demo(void const *const args);
void countSem_demo(void const *const args);
void demo_end(void const * const args);
void scheduler_intro(void const * const args);
void mutex_intro(void const * const args);
void sleep_intro(void const * const args);
void semaphore_intro(void const * const args);

#endif /*_DEMO_H_*/

