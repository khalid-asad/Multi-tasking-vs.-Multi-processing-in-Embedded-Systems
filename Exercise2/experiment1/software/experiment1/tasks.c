// Copyright by Adam Kinsman, Henry Ko and Nicola Nicolici
// Developed for the Embedded Systems course (COE4DS4)
// Department of Electrical and Computer Engineering
// McMaster University
// Ontario, Canada

#include "define.h"

// For the performance counter
void *performance_name = PERFORMANCE_COUNTER_0_BASE;

// Definition of semaphore for PBs
OS_EVENT *PBSemaphore[4];

// Definition of mailboxes
OS_EVENT *MboxGenArrayStart[2];
/*OS_EVENT *MboxFind0Start[2];
OS_EVENT *MboxFind1Start[2];
OS_EVENT *MboxFind0Done[2];
OS_EVENT *MboxFind1Done[2];
OS_EVENT *MboxFindMinMax;*/

OS_EVENT *MboxTask1Start[3];
OS_EVENT *MboxTask2Start[3];
OS_EVENT *MboxTask3Start[3];
OS_EVENT *MboxTask4Start[3];
OS_EVENT *MboxTask1Done[2];
OS_EVENT *MboxTask2Done[2];
OS_EVENT *MboxTask3Done[2];
OS_EVENT *MboxTask4Done[2];

// memory partition
OS_MEM	*MemoryPartition;

// Definition of task stacks
OS_STK	  initialize_task_stk[TASK_STACKSIZE];
OS_STK	  task_launcher_stk[TASK_STACKSIZE];
OS_STK	  sorting_stk[TASK_STACKSIZE];
OS_TCB	  sorting_tcb;

OS_STK	  gen_array_stk[2][TASK_STACKSIZE];
OS_TCB	  gen_array_tcb[2];
/*OS_STK	  find_min_stk[2][TASK_STACKSIZE];
OS_TCB	  find_min_tcb[2];
OS_STK	  find_max_stk[2][TASK_STACKSIZE];
OS_TCB	  find_max_tcb[2];
OS_STK	  find_min_max_stk[TASK_STACKSIZE];
OS_TCB	  find_min_max_tcb;*/
OS_STK	  task_stk[4][TASK_STACKSIZE];
OS_TCB	  task_tcb[4];

int time[6];

// The sorting task
// It has the highest priority
// But it initiates other tasks by posting a message in the corresponding mailboxes
// Then it wait for the result to be computed by monitoring other mailboxes
// In the mean time, it'll suspend itself to free up the processor
void sorting(void* pdata) {
	INT16U *min_value, *max_value;
	INT8U return_code = OS_NO_ERR;
	int i = 0, j = 0;
	//int *data_array[2];
	int (*array_A)[TEST_SIZE];
	int (*array_B)[TEST_SIZE];
	int (*array_C)[TEST_SIZE];
	//int arr_C[32][32];
	int (*task1)[TEST_SIZE], (*task2)[TEST_SIZE], (*task3)[TEST_SIZE], (*task4)[TEST_SIZE];
	int total_time = 0;

	while (1) {
		// Delay for 2 secs so that the message from this task will be displayed last
		// If not delay, since it has highest priority, the wait message for PB0 will be displayed first
		OSTimeDlyHMSM(0, 0, 2, 0);

		printf("Waiting for PB0 to determine C = A*B each with %d entries.\n", ARRAY_SIZE);
		OSSemPend(PBSemaphore[0], 0, &return_code);
		alt_ucosii_check_return_code(return_code);

		printf("Sorting started\n");

		// Get the memory block from memory partition, and pass that memory block to other task through mailboxes
		// to start data generation
		array_A = OSMemGet(MemoryPartition, &return_code);
		alt_ucosii_check_return_code(return_code);
		return_code = OSMboxPost(MboxGenArrayStart[0], (void *)(array_A));
		alt_ucosii_check_return_code(return_code);

		array_B = OSMemGet(MemoryPartition, &return_code);
		alt_ucosii_check_return_code(return_code);
		return_code = OSMboxPost(MboxGenArrayStart[1], (void *)(array_B));
		alt_ucosii_check_return_code(return_code);
		
		array_C = OSMemGet(MemoryPartition, &return_code);
		alt_ucosii_check_return_code(return_code);
		return_code = OSMboxPost(MboxGenArrayStart[2], (void *)(array_C));
		alt_ucosii_check_return_code(return_code);

		task1 = OSMemGet(MemoryPartition, &return_code);
		alt_ucosii_check_return_code(return_code);
		return_code = OSMboxPost(MboxTask1Start[2], (void *)(task1));
		alt_ucosii_check_return_code(return_code);

		task2 = OSMemGet(MemoryPartition, &return_code);
		alt_ucosii_check_return_code(return_code);
		return_code = OSMboxPost(MboxTask2Start[2], (void *)(task2));
		alt_ucosii_check_return_code(return_code);

		task3 = OSMemGet(MemoryPartition, &return_code);
		alt_ucosii_check_return_code(return_code);
		return_code = OSMboxPost(MboxTask3Start[2], (void *)(task3));
		alt_ucosii_check_return_code(return_code);

		task4 = OSMemGet(MemoryPartition, &return_code);
		alt_ucosii_check_return_code(return_code);
		return_code = OSMboxPost(MboxTask4Start[2], (void *)(task4));
		alt_ucosii_check_return_code(return_code);

		//PERF_RESET(performance_name);

		// Start the performance counter
		//PERF_START_MEASURING(performance_name);

		printf("-waiting for tasks to complete...\n");

		task1 = (int *)OSMboxPend(MboxTask1Done[0], 0, &return_code);
		alt_ucosii_check_return_code(return_code);

		task2 = (int *)OSMboxPend(MboxTask2Done[0], 0, &return_code);
		alt_ucosii_check_return_code(return_code);

		task3 = (int *)OSMboxPend(MboxTask3Done[0], 0, &return_code);
		alt_ucosii_check_return_code(return_code);

		task4 = (int *)OSMboxPend(MboxTask4Done[0], 0, &return_code);
		alt_ucosii_check_return_code(return_code);

		OSTimeDlyHMSM(0, 0, 5, 0);

		for(i = 0; i < TEST_SIZE; i++){
			for(j = 0; j < TEST_SIZE; j++){
				if ((i < TEST_SIZE/2)&&(j < TEST_SIZE/2)) array_C[i][j] = task1[i][j];
				else if ((i >= TEST_SIZE/2)&&(j < TEST_SIZE/2)) array_C[i][j] = task2[i][j];
				else if ((i < TEST_SIZE/2)&&(j >= TEST_SIZE/2)) array_C[i][j] = task3[i][j];
				else if ((i >= TEST_SIZE/2)&&(j >= TEST_SIZE/2)) array_C[i][j] = task4[i][j];
			}
		}

		/*for(i=0;i<16;i++) for(j=0;j<16;j++) array_C[i][j]=task1[i][j];
		for(i=16;i<TEST_SIZE;i++) for(j=0;j<16;j++) array_C[i][j]=task2[i][j];
		for(i=0;i<16;i++) for(j=16;j<TEST_SIZE;j++) array_C[i][j]=task3[i][j];
		for(i=16;i<TEST_SIZE;i++) for(j=16;j<TEST_SIZE;j++) array_C[i][j]=task4[i][j];*/

		// Stop the performance counter
		//PERF_STOP_MEASURING(performance_name);

		printf("\n");

		for(i = 0; i < TEST_SIZE; i++){
			for(j = 0; j < TEST_SIZE; j++) printf("%d\t", array_C[i][j]);
			printf("\n");
		}

		for(i = 0; i < 6; i++) total_time += time[i];

		printf("Total Sorting Time Before Printing Array: %d\n", total_time);


		return_code = OSMemPut(MemoryPartition, (void *)array_A);
		alt_ucosii_check_return_code(return_code);

		return_code = OSMemPut(MemoryPartition, (void *)array_B);
		alt_ucosii_check_return_code(return_code);

		return_code = OSMemPut(MemoryPartition, (void *)(array_C));
		alt_ucosii_check_return_code(return_code);

		return_code = OSMemPut(MemoryPartition, (void *)(task1));
		alt_ucosii_check_return_code(return_code);

		return_code = OSMemPut(MemoryPartition, (void *)(task2));
		alt_ucosii_check_return_code(return_code);

		return_code = OSMemPut(MemoryPartition, (void *)(task3));
		alt_ucosii_check_return_code(return_code);

		return_code = OSMemPut(MemoryPartition, (void *)(task4));
		alt_ucosii_check_return_code(return_code);

		OSTimeDlyHMSM(0, 0, 5, 0);
	}
}

// gen_array_0 task
// It randomly generates numbers in the range of 0-10000 (16 bits)
// And stores them in the data array obtained from the mailbox
void gen_array_0(void* pdata) {
	int (*data_array)[TEST_SIZE];
	INT8U return_code = OS_NO_ERR;
	int i = 0, j = 0;
	INT16U min_value, max_value;
	int switches;

	while (1) {
		printf("gen_array_0 wait for start.\n");
		data_array = (int *)OSMboxPend(MboxGenArrayStart[0], 0, &return_code);
		alt_ucosii_check_return_code(return_code);

		printf("gen_array_0 started.\n");

		//max_value = 0;
		//min_value = 65535;

		// Get the seed from the switches
		switches = IORD(SWITCH_I_BASE, 0);

		srand(switches);

		PERF_RESET(performance_name);

		// Start the performance counter
		PERF_START_MEASURING(performance_name);

		printf("\nArray A:\n");
		// Start generation
		for (i = 0; i < TEST_SIZE; i++) {
			for (j = 0; j < TEST_SIZE; j++){
				//data_array[i][j] = 1;
				data_array[i][j] = (rand() % 20001) - 10000;
				printf("%d\t", data_array[i][j]);
			}
			printf("\n");
		}
		
		// Start the performance counter
		PERF_STOP_MEASURING(performance_name);

		time[0] = perf_get_section_time(performance_name,0);

		printf("gen_array_0 done. Generation time: %d \n", time[0]);

		// When data generation is finished, the data array is posted to
		// two different mailboxes, so that the two tasks for finding
		// the min and max values in the array will be initiated
		//return_code = OSMboxPost(MboxFind0Start, (void *)(data_array));
		//alt_ucosii_check_return_code(return_code);

		return_code = OSMboxPost(MboxTask1Start[0], (void *)(data_array));
		alt_ucosii_check_return_code(return_code);

		return_code = OSMboxPost(MboxTask2Start[0], (void *)(data_array));
		alt_ucosii_check_return_code(return_code);

		return_code = OSMboxPost(MboxTask3Start[0], (void *)(data_array));
		alt_ucosii_check_return_code(return_code);

		return_code = OSMboxPost(MboxTask4Start[0], (void *)(data_array));
		alt_ucosii_check_return_code(return_code);

		OSTimeDlyHMSM(0, 0, 5, 0);
	}
}

// gen_array_1 task
// It randomly generates numbers in the range of 0-400 (16 bits)
// And stores them in the data array obtained from the mailbox
void gen_array_1(void* pdata) {
	int (*data_array)[TEST_SIZE];
	INT8U return_code = OS_NO_ERR;
	int i = 0, j = 0;
	INT16U min_value, max_value;
	int switches;

	while (1) {
		printf("gen_array_1 wait for start.\n");
		data_array = (int *)OSMboxPend(MboxGenArrayStart[1], 0, &return_code);
		alt_ucosii_check_return_code(return_code);

		printf("gen_array_1 started.\n");

		//max_value = 0;
		//min_value = 65535;

		// Get the seed from the switches
		switches = IORD(SWITCH_I_BASE, 0);

		srand(~switches);

		PERF_RESET(performance_name);

		// Start the performance counter
		PERF_START_MEASURING(performance_name);

		printf("\nArray B:\n");
		// Start generation
		for (i = 0; i < TEST_SIZE; i++) {
			for (j = 0; j < TEST_SIZE; j++){
				//data_array[i][j] = 1;
				data_array[i][j] = (rand() % 801) - 400;
				printf("%d\t", data_array[i][j]);
			}
			printf("\n");
		}

		// Start the performance counter
		PERF_STOP_MEASURING(performance_name);

		time[1] = perf_get_section_time(performance_name,0);

		printf("gen_array_1 done. Generation time: %d \n", time[1]);

		// When data generation is finished, the data array is posted to
		// two different mailboxes, so that the two tasks for finding
		// the min and max values in the array will be initiated

		return_code = OSMboxPost(MboxTask1Start[1], (void *)(data_array));
		alt_ucosii_check_return_code(return_code);

		return_code = OSMboxPost(MboxTask2Start[1], (void *)(data_array));
		alt_ucosii_check_return_code(return_code);

		return_code = OSMboxPost(MboxTask3Start[1], (void *)(data_array));
		alt_ucosii_check_return_code(return_code);

		return_code = OSMboxPost(MboxTask4Start[1], (void *)(data_array));
		alt_ucosii_check_return_code(return_code);

		OSTimeDlyHMSM(0, 0, 5, 0);
	}
}

// run_task_1 task
// It obtains the data array and Array A and B from the mailbox
// and then multiplies the top half of rows from Array A with
// the left half of the columnns from Array B
void run_task_1(void* pdata) {
	INT16U value[2];
	int (*data_array)[TEST_SIZE];
	INT8U return_code = OS_NO_ERR;
	int i = 0, row = 0, col = 0;
	int (*array_A)[TEST_SIZE], (*array_B)[TEST_SIZE];
	int t1_total = 0;

	while (1) {
		printf("Task 1 wait for start\n");
		array_A = (int *)OSMboxPend(MboxTask1Start[0], 0, &return_code);
		alt_ucosii_check_return_code(return_code);
		array_B = (int *)OSMboxPend(MboxTask1Start[1], 0, &return_code);
		alt_ucosii_check_return_code(return_code);
		data_array = (int *)OSMboxPend(MboxTask1Start[2], 0, &return_code);
		alt_ucosii_check_return_code(return_code);
		printf("Task 1 started.\n");

		PERF_RESET(performance_name);

		// Start the performance counter
		PERF_START_MEASURING(performance_name);

		for (row = 0; row < TEST_SIZE/2; row++) {
			for (col = 0; col < TEST_SIZE/2; col++) {
				t1_total = 0;
				for (i = 0; i < TEST_SIZE; i++) {
					t1_total += (array_A[row][i])*(array_B[i][col]);
				}
				data_array[row][col] = t1_total;
			}
		}

		// Stop performance counter
		PERF_END(performance_name, 1);

		time[2] = perf_get_section_time(performance_name,0);

		printf("Task 1 done. Task 1 time: %d \n", time[2]);

		// Post the min value to the mailbox for other task
		return_code = OSMboxPost(MboxTask1Done[0], (void *)(data_array));
		alt_ucosii_check_return_code(return_code);

		OSTimeDlyHMSM(0, 0, 10, 0);
	}
}

// run_task_2 task
// It obtains the data array and Array A and B from the mailbox
// and then multiplies the top half of rows from Array A with
// the left half of the columnns from Array B
void run_task_2(void* pdata) {
	INT16U value[2];
	int (*data_array)[TEST_SIZE];
	INT8U return_code = OS_NO_ERR;
	int i = 0, row = 0, col = 0;
	int (*array_A)[TEST_SIZE], (*array_B)[TEST_SIZE];
	int t2_total = 0;

	while (1) {
		printf("Task 2 wait for start.\n");
		array_A = (int *)OSMboxPend(MboxTask2Start[0], 0, &return_code);
		alt_ucosii_check_return_code(return_code);
		array_B = (int *)OSMboxPend(MboxTask2Start[1], 0, &return_code);
		alt_ucosii_check_return_code(return_code);
		data_array = (int *)OSMboxPend(MboxTask2Start[2], 0, &return_code);
		alt_ucosii_check_return_code(return_code);
		printf("Task 2 started.\n");

		PERF_RESET(performance_name);

		// Start the performance counter
		PERF_START_MEASURING(performance_name);

		for (row = TEST_SIZE/2; row < TEST_SIZE; row++) {
			for (col = 0; col < TEST_SIZE/2; col++) {
				t2_total = 0;
				for (i = 0; i < TEST_SIZE; i++) {
					t2_total += (array_A[row][i])*(array_B[i][col]);
				}
				data_array[row][col] = t2_total;
			}
		}

		// Stop performance counter
		PERF_END(performance_name, 1);

		time[3] = perf_get_section_time(performance_name,0);

		printf("Task 2 done. Task 2 time: %d \n", time[3]);

		// Post the min value to the mailbox for other task
		return_code = OSMboxPost(MboxTask2Done[0], (void *)(data_array));
		alt_ucosii_check_return_code(return_code);

		OSTimeDlyHMSM(0, 0, 10, 0);
	}
}

// run_task_3 task
// It obtains the data array and Array A and B from the mailbox
// and then multiplies the bottom half of rows from Array A with
// the right half of the columns from Array B
void run_task_3(void* pdata) {
	INT8U return_code = OS_NO_ERR;
	int (*data_array)[TEST_SIZE];
	int i = 0, row = 0, col = 0;
	int (*array_A)[TEST_SIZE], (*array_B)[TEST_SIZE];
	int t3_total = 0;

	while (1) {
		printf("Task 3 wait for start.\n");
		array_A = (int *)OSMboxPend(MboxTask3Start[0], 0, &return_code);
		alt_ucosii_check_return_code(return_code);
		array_B = (int *)OSMboxPend(MboxTask3Start[1], 0, &return_code);
		alt_ucosii_check_return_code(return_code);
		data_array = (int *)OSMboxPend(MboxTask3Start[2], 0, &return_code);
		alt_ucosii_check_return_code(return_code);
		printf("Task 3 started.\n");

		PERF_RESET(performance_name);

		// Start the performance counter
		PERF_START_MEASURING(performance_name);

		for (row = 0; row < TEST_SIZE/2; row++) {
			for (col = TEST_SIZE/2; col < TEST_SIZE; col++) {
				t3_total = 0;
				for (i = 0; i < TEST_SIZE; i++) {
					t3_total += (array_A[row][i])*(array_B[i][col]);
				}
				data_array[row][col] = t3_total;
			}
		}

		// Stop performance counter
		PERF_END(performance_name, 1);

		time[4] = perf_get_section_time(performance_name,0);

		printf("Task 3 done. Task 3 time: %d \n", time[4]);

		// Post the min value to the mailbox for other task
		return_code = OSMboxPost(MboxTask3Done[0], (void *)(data_array));
		alt_ucosii_check_return_code(return_code);

		OSTimeDlyHMSM(0, 0, 10, 0);
	}
}

// run_task_1 task
// It obtains the data array and Array A and B from the mailbox
// and then multiplies the bottom half of rows from Array A with
// the right half of the columnns from Array B
void run_task_4(void* pdata) {
	INT8U return_code = OS_NO_ERR;
	int (*data_array)[TEST_SIZE];
	int i = 0, row = 0, col = 0;
	int (*array_A)[TEST_SIZE], (*array_B)[TEST_SIZE];
	int t4_total = 0;

	while (1) {
		printf("Task 4 wait for start.\n");
		array_A = (int *)OSMboxPend(MboxTask4Start[0], 0, &return_code);
		alt_ucosii_check_return_code(return_code);
		array_B = (int *)OSMboxPend(MboxTask4Start[1], 0, &return_code);
		alt_ucosii_check_return_code(return_code);
		data_array = (int *)OSMboxPend(MboxTask4Start[2], 0, &return_code);
		alt_ucosii_check_return_code(return_code);
		printf("Task 4 started.\n");

		PERF_RESET(performance_name);

		// Start the performance counter
		PERF_START_MEASURING(performance_name);

		for (row = TEST_SIZE/2; row < TEST_SIZE; row++) {
			for (col = TEST_SIZE/2; col < TEST_SIZE; col++) {
				t4_total = 0;
				for (i = 0; i < TEST_SIZE; i++) {
					t4_total += (array_A[row][i])*(array_B[i][col]);
				}
				data_array[row][col] = t4_total;
			}
		}

		// Stop performance counter
		PERF_END(performance_name, 1);
		
		time[5] = perf_get_section_time(performance_name,0);

		printf("Task 4 done. Task 4 time: %d \n", time[5]);

		// Post the min value to the mailbox for other task
		return_code = OSMboxPost(MboxTask4Done[0], (void *)(data_array));
		alt_ucosii_check_return_code(return_code);

		OSTimeDlyHMSM(0, 0, 10, 0);
	}
}


// Task launcher
// It creates all the custom tasks
// And then it deletes itself
void task_launcher(void *pdata) {
	INT8U return_code = OS_NO_ERR;

	#if OS_CRITICAL_METHOD == 3
			OS_CPU_SR cpu_sr;
	#endif

	printf("Starting task launcher...\n");
	while (1) {
		OS_ENTER_CRITICAL();
		printf("Creating tasks...\n");

		return_code = OSTaskCreateExt(sorting,
			NULL,
			(void *)&sorting_stk[TASK_STACKSIZE-1],
			SORTING_PRIORITY,
			SORTING_PRIORITY,
			&sorting_stk[0],
			TASK_STACKSIZE,
			&sorting_tcb,
			0);
		alt_ucosii_check_return_code(return_code);

		return_code = OSTaskCreateExt(gen_array_0,
			NULL,
			(void *)&gen_array_stk[0][TASK_STACKSIZE-1],
			GEN_ARRAY_0_PRIORITY,
			GEN_ARRAY_0_PRIORITY,
			&gen_array_stk[0][0],
			TASK_STACKSIZE,
			&gen_array_tcb[0],
			0);
		alt_ucosii_check_return_code(return_code);

		return_code = OSTaskCreateExt(gen_array_1,
			NULL,
			(void *)&gen_array_stk[1][TASK_STACKSIZE-1],
			GEN_ARRAY_1_PRIORITY,
			GEN_ARRAY_1_PRIORITY,
			&gen_array_stk[1][0],
			TASK_STACKSIZE,
			&gen_array_tcb[1],
			0);
		alt_ucosii_check_return_code(return_code);

		return_code = OSTaskCreateExt(run_task_1,
			NULL,
			(void *)&task_stk[0][TASK_STACKSIZE-1],
			TASK_1_PRIORITY,
			TASK_1_PRIORITY,
			&task_stk[0][0],
			TASK_STACKSIZE,
			&task_tcb[0],
			0);
		alt_ucosii_check_return_code(return_code);

		return_code = OSTaskCreateExt(run_task_2,
			NULL,
			(void *)&task_stk[1][TASK_STACKSIZE-1],
			TASK_2_PRIORITY,
			TASK_2_PRIORITY,
			&task_stk[1][0],
			TASK_STACKSIZE,
			&task_tcb[1],
			0);
		alt_ucosii_check_return_code(return_code);

		return_code = OSTaskCreateExt(run_task_3,
			NULL,
			(void *)&task_stk[2][TASK_STACKSIZE-1],
			TASK_3_PRIORITY,
			TASK_3_PRIORITY,
			&task_stk[2][0],
			TASK_STACKSIZE,
			&task_tcb[2],
			0);
		alt_ucosii_check_return_code(return_code);

		return_code = OSTaskCreateExt(run_task_4,
			NULL,
			(void *)&task_stk[3][TASK_STACKSIZE-1],
			TASK_4_PRIORITY,
			TASK_4_PRIORITY,
			&task_stk[3][0],
			TASK_STACKSIZE,
			&task_tcb[3],
			0);
		alt_ucosii_check_return_code(return_code);

		printf("Finish creating tasks...\n");

		printf("\n");
		OSTimeDlyHMSM(0, 0, 10, 0);

		return_code = OSTaskDel(OS_PRIO_SELF);
		alt_ucosii_check_return_code(return_code);

		OS_EXIT_CRITICAL();
	}
}
