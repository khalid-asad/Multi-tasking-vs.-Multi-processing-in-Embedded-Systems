// Copyright by Adam Kinsman, Henry Ko and Nicola Nicolici
// Developed for the Embedded Systems course (COE4DS4)
// Department of Electrical and Computer Engineering
// McMaster University
// Ontario, Canada

#include "define.h"

// Definition of uCOS data structures
extern OS_EVENT *PBSemaphore[];

extern OS_FLAG_GRP *SDCardFlag;

// Definition of mutex
extern OS_EVENT *SDMutex;

// Definition of mailbox
extern OS_EVENT *YMailbox[4];
extern OS_EVENT *YImageWidthMailbox[4];
extern OS_EVENT *YImageHeightMailbox[4];
extern OS_EVENT *ReadImageWidthMailbox[4];
extern OS_EVENT *ReadImageHeightMailbox[4];
extern OS_EVENT *WriteImageWidthMailbox[4];
extern OS_EVENT *WriteImageHeightMailbox[4];

// Definition of message queue
extern OS_EVENT *SDWriteQueue;
extern OS_EVENT *SDReadQueue;
extern OS_EVENT *SDReadQueue1;
extern OS_EVENT *SDReadQueue2;
extern OS_EVENT *SDReadQueue3;
extern OS_EVENT *SDReadQueue4;


// memory partition
extern OS_MEM	*MemoryPartition;

// Definition of task stacks
extern OS_STK	 initialize_task_stk[TASK_STACKSIZE];
extern OS_STK	 task_launcher_stk[TASK_STACKSIZE];

// Local function prototypes
int init_OS_data_structs(void);
int init_create_tasks(void);

char line_buf[4][LINE_LEN];
void *ReadQueue[QUEUE_LEN]; 
void *WriteQueue[QUEUE_LEN]; 

// Initialization task for uCOS
void initialize_task(void* pdata) {
	INT8U return_code = OS_NO_ERR;

	// Initialize statistic counters in OS
	OSStatInit();

	// create os data structures
	init_OS_data_structs();

	// create the tasks
	init_create_tasks();

	// This task is deleted because there is no need for it to run again
	return_code = OSTaskDel(OS_PRIO_SELF);
	alt_ucosii_check_return_code(return_code);

	while (1);
}

// The main function, it initializes the hardware, and create the initialization task,
// then it starts uCOS, and never returns
int main(void) {
	INT8U return_code = OS_NO_ERR;
	
	printf("Start main...\n");

	init_button_irq();
	printf("PB initialized...\n");

	seg7_show(SEG7_DISPLAY_0_BASE,SEG7_VALUE);
	printf("SEG7 initialized...\n");

    sd_card_open_dev();
	printf("Opened SD card device...\n");

	PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
	printf("Reset performance counter...\n");

	OSInit();

	return_code = OSTaskCreateExt(initialize_task,
					NULL,
					(void *)&initialize_task_stk[TASK_STACKSIZE],
					INITIALIZE_TASK_PRIORITY,
					INITIALIZE_TASK_PRIORITY,
					initialize_task_stk,
					TASK_STACKSIZE,
					NULL,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	alt_ucosii_check_return_code(return_code);
	printf("Starting uCOS...\n");

	OSStart();
	return 0;
}

/* This function simply creates a message queue and a semaphore
 */

int init_OS_data_structs(void)
{
	int i, x;
	INT8U return_code = OS_NO_ERR;

	printf("Init data structs...\n");

	for (i = 0; i < NUM_PB_BUTTON; i++)
		PBSemaphore[i] = OSSemCreate(0);
	
	for (x = 0; x < 4; x++){

		YMailbox[x] = OSMboxCreate((void *)0);
		YImageWidthMailbox[x] = OSMboxCreate((void *)0);
		YImageHeightMailbox[x] = OSMboxCreate((void *)0);
		ReadImageWidthMailbox[x] = OSMboxCreate((void *)0);
		ReadImageHeightMailbox[x] = OSMboxCreate((void *)0);
		WriteImageWidthMailbox[x] = OSMboxCreate((void *)0);
		WriteImageHeightMailbox[x] = OSMboxCreate((void *)0);

	}


	MemoryPartition = OSMemCreate(&line_buf[0][0], 16, LINE_LEN, &return_code);
	
	SDReadQueue = OSQCreate(&ReadQueue[0], QUEUE_LEN);
	/*SDReadQueue1 = OSQCreate(&ReadQueue[1], QUEUE_LEN);
	SDReadQueue2 = OSQCreate(&ReadQueue[2], QUEUE_LEN);
	SDReadQueue3 = OSQCreate(&ReadQueue[3], QUEUE_LEN);*/
	SDWriteQueue = OSQCreate(&WriteQueue, QUEUE_LEN);
	
	SDCardFlag = OSFlagCreate(0, &return_code);
	
	SDMutex = OSMutexCreate(SD_MUTEX_PRIORITY, &return_code);

	return 0;
}

// This function creates the first task in uCOS
int init_create_tasks(void) {
	INT8U return_code = OS_NO_ERR;

	printf("Creating task launcher...\n");
	return_code = OSTaskCreateExt(task_launcher,
					NULL,
					(void *)&task_launcher_stk[TASK_STACKSIZE],
					TASK_LAUNCHER_PRIORITY,
					TASK_LAUNCHER_PRIORITY,
					task_launcher_stk,
					TASK_STACKSIZE,
					NULL,
					0);
	alt_ucosii_check_return_code(return_code);

	return 0;
}
