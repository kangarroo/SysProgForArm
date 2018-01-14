#include "demo.h"
#include "sleep.h"
#include <stdio.h>

/*Scheduler Demo
This function demonstrates the function of the fixed priority scheduler.
Showing it can add tasks, remove tasks and run pre-emptivly and in priority
order. Successfull output should show multiple task switching while staying in
the same priority level. 

Example Output: (H = HIGH PRIORITY, M = MEDIUM PRIORITY, L = LOW PRIORITY)
H1,H2,H1,H2,M1,M2,M1,M2,L1,L2,L1,L2
Where all tasks are set to run twice

In the demo the High Priority tasks print 1 & 2, Medium Print 3 & 4 and Low
Priority prints 5 & 6. All tasks have been set to print their output 10 times*/
static void scheduler_H1(void const *const args){
	for(int i=0;i<10;i++){
		printf("1");
	}
}
static void scheduler_H2(void const *const args){
	for(int i=0;i<10;i++){
		printf("2");
	}
}
static void scheduler_M1(void const *const args){
	for(int i=0;i<10;i++){
		printf("3");
	}
}

static void scheduler_M2(void const *const args){
	for(int i=0;i<10;i++){
		printf("4");
	}
}

static void scheduler_L1(void const *const args){
	for(int i=0;i<10;i++){
		printf("5");
	}
	OS_binarySemaphore_release(&DEMO_SEMAPHORE); //Required to Syncronise the demo tasks
}

static void scheduler_L2(void const *const args){
	for(int i=0;i<10;i++){
		printf("6");
	}
}

void scheduler_demo(void const *const args) {
	OS_binarySemaphore_take(&DEMO_SEMAPHORE);
	
	//Initialise Demo Tasks
	__align(8)
	static uint32_t sH1[64],sH2[64],sM1[64],sM2[64],sL1[64],sL2[64];
	static OS_TCB_t tH1,tH2,tM1,tM2,tL1,tL2;
	
	//High Level Tasks
	OS_initialiseTCB(&tH1, sH1+64, scheduler_H1,NULL, 0);
	OS_initialiseTCB(&tH2, sH2+64, scheduler_H2,NULL, 0);
	//Medium Level Tasks
	OS_initialiseTCB(&tM1, sM1+64, scheduler_M1,NULL, 1);
	OS_initialiseTCB(&tM2, sM2+64, scheduler_M2,NULL, 1);
	//Low Level Tasks
	OS_initialiseTCB(&tL1, sL1+64, scheduler_L1,NULL, 2);
	OS_initialiseTCB(&tL2, sL2+64, scheduler_L2,NULL, 2);
	
	OS_addTask(&tH1);
	OS_addTask(&tH2);
	OS_addTask(&tM1);
	OS_addTask(&tM2);
	OS_addTask(&tL1);
	OS_addTask(&tL2);
	
	
}

/*Mutex Demo
This demonstrates the use of Mutexes in DocetOS, the input to
this demo will be the same as in the scheduler demo, however, the
tasks have been mutexed so the output will be in order
Example Output: (H = High Priority, M = Medium Priority, L = Low Priority
H1,H1,H2,H2,M1,M1,M2,M2,L1,L1,L2,L2
*/
static void mutex_H1(void const *const args){
	OS_mutex_aquire(&mutex);
	for(int i=0;i<10;i++){
		printf("1");
	}
	OS_mutex_release(&mutex);
}
static void mutex_H2(void const *const args){
	OS_mutex_aquire(&mutex);
	for(int i=0;i<10;i++){
		printf("2");
	}
	OS_mutex_release(&mutex);
}
static void mutex_M1(void const *const args){
	OS_mutex_aquire(&mutex);
	for(int i=0;i<10;i++){
		printf("3");
	}
	OS_mutex_release(&mutex);
}
static void mutex_M2(void const *const args){
	OS_mutex_aquire(&mutex);
	for(int i=0;i<10;i++){
		printf("4");
	}
	OS_mutex_release(&mutex);
}
static void mutex_L1(void const *const args){
	OS_mutex_aquire(&mutex);
	for(int i=0;i<10;i++){
		printf("5");
	}
	OS_mutex_release(&mutex);
}
static void mutex_L2(void const *const args){
	OS_mutex_aquire(&mutex);
	for(int i=0;i<10;i++){
		printf("6");
	}
	OS_mutex_release(&mutex);
	OS_binarySemaphore_release(&DEMO_SEMAPHORE);
}


void mutex_demo(void const *const args) {
	OS_binarySemaphore_take(&DEMO_SEMAPHORE);
	//Initialise Demo Tasks
	__align(8)
	static uint32_t mH1[64],mH2[64],mM1[64],mM2[64],mL1[64],mL2[64];
	static OS_TCB_t mtH1,mtH2,mtM1,mtM2,mtL1,mtL2;
	
	//High Level Tasks
	OS_initialiseTCB(&mtH1, mH1+64, mutex_H1,NULL, 0);
	OS_initialiseTCB(&mtH2, mH2+64, mutex_H2,NULL, 0);
	//Medium Level Tasks
	OS_initialiseTCB(&mtM1, mM1+64, mutex_M1,NULL, 1);
	OS_initialiseTCB(&mtM2, mM2+64, mutex_M2,NULL, 1);
	//Low Level Tasks
	OS_initialiseTCB(&mtL1, mL1+64, mutex_L1,NULL, 2);
	OS_initialiseTCB(&mtL2, mL2+64, mutex_L2,NULL, 2);
	
	OS_initialiseMutex(&mutex);
	
	OS_addTask(&mtH1);
	OS_addTask(&mtH2);
	OS_addTask(&mtM1);
	OS_addTask(&mtM2);
	OS_addTask(&mtL1);
	OS_addTask(&mtL2);
}

/*Sleep Demo
This demonstrates the sleep functionaly of DocetOS. To do this
the same tasks will run as in the scheduler demo, however they will
have a sleep added, meaning that the order of them being run will change,
as sleeping tasks are removed from the scheduler.
Example Output: (H=High, M=Medium, L=Low)
M1,H1,H1,M1,M2,H2,H2,M2...
Where the high priority tasks are waking up between the middle priority tasks.

In this demo the the low priority tasks have the shortest wait and so get
printed first, next the medium priority both wake up at the same time so
they get scheduled as normal, as can be by their task switching. Lastly the
high priority tasks have the longest wait, so they get released last*/
static void sleep_H1(void const *const args){
	OS_Sleep(15);
	for(int i=0;i<10;i++){
		printf("1");
	}
}
static void sleep_H2(void const *const args){
	OS_Sleep(20);
	for(int i=0;i<10;i++){
		printf("2");
	}
	OS_binarySemaphore_release(&DEMO_SEMAPHORE);
}
static void sleep_M1(void const *const args){
	OS_Sleep(10);
	for(int i=0;i<10;i++){
		printf("3");
	}
}
static void sleep_M2(void const *const args){
	OS_Sleep(10);
	for(int i=0;i<10;i++){
		printf("4");
	}
}
static void sleep_L1(void const *const args){
	OS_Sleep(5);
	for(int i=0;i<10;i++){
		printf("5");
	}
}
static void sleep_L2(void const *const args){
	for(int i=0;i<10;i++){
		printf("6");
	}
}

void sleep_demo(void const *const args) {
	OS_binarySemaphore_take(&DEMO_SEMAPHORE);
	//Initialise Demo Tasks
	__align(8)
	static uint32_t slH1[64],slH2[64],slM1[64],slM2[64],slL1[64],slL2[64];
	static OS_TCB_t sltH1,sltH2,sltM1,sltM2,sltL1,sltL2;

	//High Level Tasks
	OS_initialiseTCB(&sltH1, slH1+64, sleep_H1,NULL, 0);
	OS_initialiseTCB(&sltH2, slH2+64, sleep_H2,NULL, 0);
	//Medium Level Tasks
	OS_initialiseTCB(&sltM1, slM1+64, sleep_M1,NULL, 1);
	OS_initialiseTCB(&sltM2, slM2+64, sleep_M2,NULL, 1);
	//Low Level Tasks
	OS_initialiseTCB(&sltL1, slL1+64, sleep_L1,NULL, 2);
	OS_initialiseTCB(&sltL2, slL2+64, sleep_L2,NULL, 2);
		
	OS_addTask(&sltH1);
	OS_addTask(&sltH2);
	OS_addTask(&sltM1);
	OS_addTask(&sltM2);
	OS_addTask(&sltL1);
	OS_addTask(&sltL2);
}

/*Semaphore Demo
Both a binary semaphore and a counting semaphore are implemented in this
system.
For the binary semaphore there are 2 tasks trying to take the semaphore,
however it can only be relased once the 3rd task is run, holding the mutex
release. 
Example Output:
1,1,1,3,2,2,2 Where task 3 holds the semaphore release, but first sleeps for a short time

For the counting semaphore, there are 5 tasks, and only 3 can use the 
semaphore at once. Everytime a task is released one of the waiting tasks
can take its place.
Example Output:
1,2,3,1,2,3,2,3,4,2,4,4 Where all the tasks are the same priority*/
//Binary Semaphore Demos
static void binSem_1(void const *const args){
	OS_binarySemaphore_take(&bin_sem);
	for(int i=0;i<10;i++){
		printf("1");
	}
}
static void binSem_2(void const *const args){
	OS_binarySemaphore_take(&bin_sem);
	for(int i=0;i<10;i++){
		printf("2");
	}
	OS_binarySemaphore_release(&DEMO_SEMAPHORE);
}
static void binSem_3(void const *const args){
	OS_Sleep(1);
	printf("3");
	OS_binarySemaphore_release(&bin_sem);
}
//Counting Semaphore Demos
static void countSem_1(void const *const args){
	OS_countingSemaphore_wait(&count_sem);
	for(int i=0;i<10;i++){
		printf("1");
	}
	OS_countingSemaphore_signal(&count_sem);
}
static void countSem_2(void const *const args){
	OS_countingSemaphore_wait(&count_sem);
	for(int i=0;i<10;i++){
		printf("2");
	}
	OS_countingSemaphore_signal(&count_sem);
}
static void countSem_3(void const *const args){
	OS_countingSemaphore_wait(&count_sem);
	for(int i=0;i<10;i++){
		printf("3");
	}
	OS_countingSemaphore_signal(&count_sem);
}
static void countSem_4(void const *const args){
	OS_countingSemaphore_wait(&count_sem);
	for(int i=0;i<10;i++){
		printf("4");
	}
	OS_countingSemaphore_signal(&count_sem);	
}
static void countSem_5(void const *const args){
	OS_countingSemaphore_wait(&count_sem);
	for(int i=0;i<10;i++){
		printf("5");
	}
	OS_countingSemaphore_signal(&count_sem);
	OS_binarySemaphore_release(&DEMO_SEMAPHORE);
}

void binSem_demo(void const *const args) {
	OS_binarySemaphore_take(&DEMO_SEMAPHORE);
	printf("\r\nBinary Semaphore Demo\r\n");
	//Initialise Demo Tasks
	__align(8)
	static uint32_t bs1[64],bs2[64],bs3[64];
	static OS_TCB_t bst1,bst2,bst3;
	
	//High Level Tasks
	OS_initialiseTCB(&bst1, bs1+64, binSem_1,NULL, 0);
	OS_initialiseTCB(&bst2, bs2+64, binSem_2,NULL, 0);
	OS_initialiseTCB(&bst3, bs3+64, binSem_3,NULL, 0);
	OS_binarySemaphore_init(&bin_sem);
	
	OS_addTask(&bst1);
	OS_addTask(&bst2);
	OS_addTask(&bst3);

}

void countSem_demo(void const *const args) {
	OS_binarySemaphore_take(&DEMO_SEMAPHORE);
	printf("\r\nCounting Semaphore Demo\r\n");
	//Initialise Demo Tasks
	__align(8)
	static uint32_t cs1[64],cs2[64],cs3[64],cs4[64],cs5[64];
	static OS_TCB_t cst1,cst2,cst3,cst4,cst5;
	
	//High Level Tasks
	OS_initialiseTCB(&cst1, cs1+64, countSem_1,NULL, 0);
	OS_initialiseTCB(&cst2, cs2+64, countSem_2,NULL, 0);
	OS_initialiseTCB(&cst3, cs3+64, countSem_3,NULL, 0);
	OS_initialiseTCB(&cst4, cs4+64, countSem_4,NULL, 0);
	OS_initialiseTCB(&cst5, cs5+64, countSem_5,NULL, 0);
	
	OS_countingSemaphore_init(&count_sem,3);
	
	OS_addTask(&cst1);
	OS_addTask(&cst2);
	OS_addTask(&cst3);
	OS_addTask(&cst4);
	OS_addTask(&cst5);
}

//Section Introductions start here
//These are loing printfs that run before each demo giving some
//information about what it does and expected output
void demo_end(void const * const args) {
	OS_binarySemaphore_take(&DEMO_SEMAPHORE);
	OS_Sleep(100);
	printf("\r\n\r\n--END OF DEMO--");
	OS_binarySemaphore_release(&DEMO_SEMAPHORE);
}

void scheduler_intro(void const * const args) {
	OS_binarySemaphore_take(&DEMO_SEMAPHORE);
	printf("\r\n\r\nScheduler Demo\r\n");
	printf("This function demonstrates the use of the scheduler. If successful "
	"the output will show mixed tasks\r\nsorted by priority levels "
	"(due to the schedulers preemptive nature), where 1 & 2 "
	"are high priority,\r\n3 & 4 are medium priority and 5 & 6 are low priority.\r\n\r\n");
	OS_binarySemaphore_release(&DEMO_SEMAPHORE);
}

void mutex_intro(void const * const args) {
	OS_binarySemaphore_take(&DEMO_SEMAPHORE);
	printf("\r\n\r\nMutex Demo\r\n");
	printf("This demonstrates the use of mutexes. The input to the scheduler"
	"is the same as during the scheduler demo,\r\nhowever this time the tasks are mutexed."
	"If run successfully the tasks should print in priority order in\r\nseparate blocks"
	"with once again 1 & 2 being Top priority, 3 & 4 being middle priority and 5 & 6 being low priority.\r\n\r\n");
	OS_binarySemaphore_release(&DEMO_SEMAPHORE);
}

void sleep_intro(void const * const args) {
	OS_binarySemaphore_take(&DEMO_SEMAPHORE);
	printf("\r\n\r\nSleep Demo\r\n");
	printf("This demonstrates the use of sleep. The input to the scheduler"
	"is the same as during the scheduler demo, however\r\nthis time the tasks have an added sleep. "
	"The tasks have been set up, so the High priority tasks 1 & 2 have the\r\nlongest waits, and "
	"the lowest priority tasks have the shortest time. The medium priority tasks both wake "
	"up at\r\nthe same time to show that they get added back to the scheduler.\r\n\r\n");
	OS_binarySemaphore_release(&DEMO_SEMAPHORE);
}

void semaphore_intro(void const * const args) {
	OS_binarySemaphore_take(&DEMO_SEMAPHORE);
	printf("\r\n\r\nSemaphore Demos\r\n");
	printf("This demonstrates the use of semaphores.\r\nIn the binary semaphore test "
				 "there are 3 tasks all of the same priority, where tasks 1 and 2 are both "
				 "trying to\r\naquire the semaphore, and task 3 relases the semaphore after a brief "
				 "sleeping period. If successful it should\r\nprint all of task 1, then run task 3 and lastly "
				 "run task 2.\r\n\r\nFor the Counting semaphores there are 5 tasks tring to aquire a counting "
			   "semaphore with 3 tokens. If successfully\r\nrun, the output will show the first 3 tasks running "
				 "normally while the other 2 wait till semaphore tokens\r\nhave been released."
				 "\r\n\r\n");
	OS_binarySemaphore_release(&DEMO_SEMAPHORE);
}
