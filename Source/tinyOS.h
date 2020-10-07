#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>
#include "tLib.h"
#include "tConfig.h"
#include "tTask.h"
#include "tEvent.h"
#include "tSem.h"
#include "tMbox.h"
#include "tMemBlock.h"

extern int scheduleLockCount;
extern tList taskTable[TINYOS_PRO_COUNT];
extern tBitmap taskProBitmap;
extern tList taskDelayList;

extern tTask * currentTask;
extern tTask * nextTask;

typedef enum _tError
{
	tErrorNoError =0,
	tErrorTimeOut =1,
	tErrorResourceUnavaliable,
	tErrorDestory,
	tErrorResourceFull,
	tErrorDelete,
}tError;

void tInitApp(void);
void task1Entry(void * param);
void task2Entry(void * param);
void task3Entry(void * param);
void task4Entry(void * param);

void taskIdleEntry(void);

void tSystemInit(void);
void tSetSysTickPeriod(uint32_t ms);
void tTaskSystemTickHandler(void);
void tTaskDelay(uint32_t delay);

uint32_t tTaskEnterCritical(void);
void tTaskExitCritical(uint32_t status);

void tTaskScheduleInit(void);
void tScheduleLockDisable(void);
void tScheduleLockEnable(void);

tTask * tTaskHighestReady(void);

void tTaskDelayListInit(void);
void tTimeTaskWait(tTask * task, uint32_t ticks);
void tTimeTaskWake(tTask * task);
void tTimeTaskRemove(tTask * task);

void tTaskReadyTableInit(void);
void tTaskScheduleReady(tTask * task);
void tTaskScheduleUnReady(tTask * task);
void tTaskReadyRemove(tTask * task);

void tTaskSuspend(tTask * task);
void tTaskResume(tTask * task);

#endif

