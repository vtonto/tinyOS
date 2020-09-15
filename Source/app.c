#include "tinyOS.h"

tTask tTask1;
tTask tTask2;
tTask tTask3;
tTask tTask4;

tTaskStack task1Stack[128];
tTaskStack task2Stack[128];
tTaskStack task3Stack[128];
tTaskStack task4Stack[128];


int task1Flag;
int task2Flag;
int task3Flag;
int task4Flag;

void tInitApp()
{
	tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Stack[128]);
	tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Stack[128]);
	tTaskInit(&tTask3, task3Entry, (void *)0x33333333, 1, &task3Stack[128]);
	tTaskInit(&tTask4, task4Entry, (void *)0x44444444, 1, &task4Stack[128]);
}

tEvent eventWaitNormal;

void task1Entry(void * param)
{
	tEventInit(&eventWaitNormal, tEventTypeUnKnow);
	for(;;)
	{
		uint32_t count = tEventWaitCount(&eventWaitNormal);
		uint32_t waitCount = tEventRemoveAll(&eventWaitNormal, (void *)0, 0);
		if(waitCount)
		{
			tTaskSchedule();
			count = tEventWaitCount(&eventWaitNormal);
		}
		task1Flag =0;
		tTaskDelay(1);
		task1Flag =1;
		tTaskDelay(1);
	}
}

void task2Entry(void * param)
{
	for(;;)
	{
		tEventWait(&eventWaitNormal, currentTask, (void *)0, 0, 0);
		tTaskSchedule();
		task2Flag =0;
		tTaskDelay(1);	
		task2Flag =1;
		tTaskDelay(1);
	}
}

void task3Entry(void * param)
{
	for(;;)
	{
		tEventWait(&eventWaitNormal, currentTask, (void *)0, 0, 0);
		tTaskSchedule();
		task3Flag =0;
		tTaskDelay(1);
		task3Flag =1;
		tTaskDelay(1);
	}
}

void task4Entry(void * param)
{
	for(;;)
	{
		tEventWait(&eventWaitNormal, currentTask, (void *)0, 0, 0);
		tTaskSchedule();
		task4Flag =0;
		tTaskDelay(1);
		task4Flag =1;
		tTaskDelay(1);
	}
}
