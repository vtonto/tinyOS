#include "tinyOS.h"
#include "ARMCM3.h"

tTask * currentTask;
tTask * nextTask;

tList taskTable[TINYOS_PRO_COUNT];
tBitmap taskProBitmap;
tList taskDelayList;

tTask tTaskIdle;
tTaskStack taskIdleStack[1024];
tTask * idleTask;

int scheduleLockCount;

int main()
{
	tSetSysTickPeriod(10);
	tSystemInit();

	tInitApp();
	tTaskInit(&tTaskIdle, taskIdleEntry, (void *)0x33333333, TINYOS_PRO_COUNT-1, &taskIdleStack[1024]);

	idleTask = &tTaskIdle;
	
	nextTask = tTaskHighestReady();
	
	tTaskRunFirst();
	
	return 0;
}



void taskIdleEntry()
{
	for(;;)
	{}
}
