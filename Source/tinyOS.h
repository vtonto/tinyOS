#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>

typedef uint32_t tTaskStack;

typedef struct _tTask
{
	tTaskStack * stack;
	//uint32_t delayTicks;
}tTask;

extern tTask * currentTask;
extern tTask * nextTask;

void tTaskInit(tTask * task, void (*entry), void * param, tTaskStack * stack);
void tTaskRunFirst(void);
void tTaskSwitch(void);
void tTaskSchedule(void);

void tSetSysTickPeriod(uint32_t ms);
void tTaskSystemTickHandler(void);
void tTaskDelay(uint32_t delay);

#endif

