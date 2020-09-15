#include "tinyOS.h"
#include "ARMCM3.h"

tTask * currentTask;
tTask * nextTask;

tList taskTable[TINYOS_PRO_COUNT];
tBitmap taskProBitmap;
tList taskDelayList;

tTask tTaskIdle;
tTaskStack taskIdleStack[TINYOS_IDLETASK_STACK];
tTask * idleTask;

int scheduleLockCount;

int main()
{
	tList * ptr = taskTable;
	tSystemInit();

	tInitApp();
	
	tTaskInit(&tTaskIdle, taskIdleEntry, (void *)0x33333333, TINYOS_PRO_COUNT-1, &taskIdleStack[TINYOS_IDLETASK_STACK]);

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
