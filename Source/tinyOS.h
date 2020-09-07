#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>

typedef uint32_t tTaskStack;

typedef struct _tTask
{
	tTaskStack * stack;
	uint32_t delayTicks;
}tTask;

extern tTask * currentTask;
extern tTask * nextTask;

void task1Entry(void * param);
void task2Entry(void * param);
void taskIdleEntry(void);

void tTaskInit(tTask * task, void (*entry), void * param, tTaskStack * stack);
void tTaskRunFirst(void);
void tTaskSwitch(void);
void tTaskSchedule(void);

void tSetSysTickPeriod(uint32_t ms);
void tTaskSystemTickHandler(void);
void tTaskDelay(uint32_t delay);

uint32_t tTaskEnterCritical(void);
void tTaskExitCritical(uint32_t status);

#endif

