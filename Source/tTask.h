#ifndef TTASK_H
#define TTASK_H

#include <stdint.h>
#include "tLib.h"

struct _tEvent;

#define TINYOS_TASK_STATE_RDY              0
#define TINYOS_TASK_STATE_DESTORYED        (1 << 1)
#define TINYOS_TASK_STATE_DELAY            (1 << 1)
#define TINYOS_TASK_STATE_SUSPEND          (1 << 3)

#define TINYOS_TASK_WAIT_MASK              (0xFF << 16)

typedef uint32_t tTaskStack;

typedef struct _tTask
{
	tTaskStack * stack;
	uint32_t delayTicks;
	uint32_t remainTicks;
	uint32_t prio;
	tNode delayNode;
	uint32_t state;
	
	tNode linkNode;
	uint32_t slice;
	
	uint32_t suspendCount;
	
	void (* clean)(void * param);
	void * cleanParam;
	uint8_t requestDeleteFlag;
	
	struct _tEvent * waitingEvent;
	void * waitingMsg;
	uint32_t eventWaitingResult;
}tTask;

typedef struct _tTaskInfo
{
	uint32_t delayTicks;
	uint32_t remainTicks;
	uint32_t prio;
	uint32_t state;
	uint32_t slice;
	uint32_t suspendCount;
	uint8_t requestDeleteFlag;
}tTaskInfo;

void tTaskInit(tTask * task, void (*entry), void * param, uint32_t prio, tTaskStack * stack);
void tTaskRunFirst(void);
void tTaskSwitch(void);
void tTaskSchedule(void);

void tTaskSetAllCleanFunc(tTask * task, void(*clean)(void * param), void * cleanParam);
void tTaskForceDelete(tTask * task);
void tTaskRequestDelete(tTask * task);
uint8_t tTaskRequestDetect(void);
void tTaskDeleteSelf(void);

void tTaskGetInfo(tTask * task, tTaskInfo * taskInfo);

#endif
