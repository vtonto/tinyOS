#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>
#include "tLib.h"
#include "tConfig.h"

typedef uint32_t tTaskStack;

#define TINYOS_TASK_STATE_RDY              0
#define TINYOS_TASK_STATE_DELAY     (1 << 1)

typedef struct _tTask
{
	tTaskStack * stack;
	uint32_t delayTicks;
	uint32_t remainTicks;
	uint32_t prio;
	tNode delayNode;
	uint32_t state;
}tTask;

extern tTask * currentTask;
extern tTask * nextTask;

void task1Entry(void * param);
void task2Entry(void * param);
void task3Entry(void * param);
void task4Entry(void * param);
void taskIdleEntry(void);

void tTaskInit(tTask * task, void (*entry), void * param, uint32_t prio, tTaskStack * stack);
void tTaskRunFirst(void);
void tTaskSwitch(void);
void tTaskSchedule(void);

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
void tTimeTaskSeqWait(tTask * task, uint32_t ticks);
void tTaskScheduleReady(tTask * task);
void tTaskScheduleUnReady(tTask * task);

#endif

