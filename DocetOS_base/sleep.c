#include "sleep.h"
#include "os_internal.h"
#include "stm32f4xx.h"

/*OS_Sleep*/
/*This function sets the state of a task to sleep for the amount to ticks
inputted as the argument to the function. */

void OS_Sleep(uint32_t sleep_time){
	OS_TCB_t *current_TCB = OS_currentTCB();
	current_TCB->sleep_time = OS_elapsedTicks() + sleep_time;
	current_TCB->state = TASK_STATE_SLEEP;
	OS_yield();
	//SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

}
