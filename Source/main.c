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

tTaskStack task1Stack[1024];
tTaskStack task2Stack[1024];
tTaskStack task3Stack[1024];

int task1Flag;
int task2Flag;
int task3Flag;

tTask tTaskIdle;
tTaskStack taskIdleStack[1024];
tTask * idleTask;

int scheduleLockCount;

int main()
{
	tSetSysTickPeriod(10);
	tTaskScheduleInit();
	
	tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Stack[1024]);
	tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Stack[1024]);
	tTaskInit(&tTask3, task3Entry, (void *)0x33333333, 1, &task3Stack[1024]);
	
	tTaskInit(&tTaskIdle, taskIdleEntry, (void *)0x33333333, TINYOS_PRO_COUNT-1, &taskIdleStack[1024]);

	
	idleTask = &tTaskIdle;
	
	nextTask = tTaskHighestReady();
	
	tTaskRunFirst();
	
	return 0;
}

tList list;
tNode node[8];

void task1Entry(void * param)
{	
	for(;;)
	{
		task1Flag =1;
		tTaskDelay(10);	
		task1Flag =0;
		tTaskDelay(10);
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
		delay();
		task2Flag =0;
		delay();
	}
}

void task3Entry(void * param)
{
	for(;;)
	{
		task3Flag =1;
		delay();
		task3Flag =0;
		delay();
	}
}

void taskIdleEntry()
{
	for(;;)
	{}
}
