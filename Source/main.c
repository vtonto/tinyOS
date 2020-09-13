#include "tinyOS.h"
#include "ARMCM3.h"

tTask * currentTask;
tTask * nextTask;

tList taskTable[TINYOS_PRO_COUNT];
tBitmap taskProBitmap;
tList taskDelayList;

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

tTask tTaskIdle;
tTaskStack taskIdleStack[1024];
tTask * idleTask;

int scheduleLockCount;

int main()
{
	tSetSysTickPeriod(10);
	tTaskScheduleInit();
	
	tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Stack[128]);
	tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Stack[128]);
	tTaskInit(&tTask3, task3Entry, (void *)0x33333333, 2, &task3Stack[128]);
	tTaskInit(&tTask4, task4Entry, (void *)0x44444444, 2, &task4Stack[128]);

	
	tTaskInit(&tTaskIdle, taskIdleEntry, (void *)0x33333333, TINYOS_PRO_COUNT-1, &taskIdleStack[1024]);

	
	idleTask = &tTaskIdle;
	
	nextTask = tTaskHighestReady();
	
	tTaskRunFirst();
	
	return 0;
}

void task1Entry(void * param)
{	
	for(;;)
	{
		task1Flag =1;
		tTaskDelay(10);	
		task1Flag =0;
		tTaskDelay(20);
	}
}

void delay()
{
	int i;
	for(i=0; i<0xFF; i++)
	{}
}

void task2Entry(void * param)
{
	for(;;)
	{
		task2Flag =1;
		tTaskDelay(20);
		task2Flag =0;
		tTaskDelay(40);
	}
}

void task3Entry(void * param)
{
	for(;;)
	{
		task3Flag =1;
		delay();
		//tTaskDelay(40);
		task3Flag =0;
		delay();
		//tTaskDelay(80);
	}
}

void task4Entry(void * param)
{
	for(;;)
	{
		task4Flag =1;
		delay();
		//tTaskDelay(80);
		task4Flag =0;
		delay();
		//tTaskDelay(160);
	}
}

void taskIdleEntry()
{
	for(;;)
	{}
}
