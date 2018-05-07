// Copyright by Adam Kinsman, Henry Ko and Nicola Nicolici
// Developed for the Embedded Systems course (COE4DS4)
// Department of Electrical and Computer Engineering
// McMaster University
// Ontario, Canada

#ifndef __task_H__
#define __task_H__

// Size of stack for each task
#define	  TASK_STACKSIZE	   2048

// Definition of Task Priorities
#define INITIALIZE_TASK_PRIORITY   4
#define SORTING_PRIORITY		  6
#define GEN_ARRAY_0_PRIORITY	  7
#define GEN_ARRAY_1_PRIORITY	  8
#define TASK_1_PRIORITY		  9
#define TASK_2_PRIORITY		  10
#define TASK_3_PRIORITY		  11
#define TASK_4_PRIORITY		  12
#define FIND_MIN_MAX_PRIORITY	  13
#define TASK_LAUNCHER_PRIORITY	  14

// Global function
void task_launcher(void *pdata);

#endif
