#include "tinyOS.h"

void tSemInit(tSem * sem, uint32_t startCount, uint32_t maxCount)
{
	sem->maxCount = maxCount;
	tEventInit(&sem->event, tEventTypeSem);
	
	if(sem->maxCount ==0)
	{
		sem->count = startCount;
	}
	else
	{
		sem->count = (sem->maxCount > startCount)? startCount : sem->maxCount;
	}
}

uint32_t tSemWait(tSem * sem, uint32_t waitTicks)
{
	uint32_t status = tTaskEnterCritical();
	
	if(sem->count >0)
	{
		--sem->count;
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else
	{
		tEventWait(&sem->event, currentTask, (void *)0, tEventTypeUnKnow, waitTicks);
		tTaskExitCritical(status);
		
		tTaskSchedule();
		return currentTask->eventWaitingResult;
	}
}

uint32_t tSemNoWaitGet(tSem * sem)
{
	uint32_t status = tTaskEnterCritical();
	
	if(sem->count >0)
	{
		--sem->count;
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else
	{
		tTaskExitCritical(status);
		return tErrorResourceUnavaliable;
	}
}
	
void tSemNotify(tSem * sem)
{
	uint32_t status = tTaskEnterCritical();
	
	if(tEventWaitCount(&sem->event) !=0)
	{
		tTask * task = tEventWake(&sem->event, (void *)0, tErrorNoError);
		if(task->prio < currentTask->prio)
		{
			tTaskSchedule();
		}
	}
	else
	{
		++sem->count;
		if( (sem->maxCount !=0) && (sem->count > sem->maxCount))
		{
			sem->count = sem->maxCount;
		}
	}
	tTaskExitCritical(status);
}
	
void tSemGetInfo(tSem * sem, tSemInfo * info)
{
	uint32_t status = tTaskEnterCritical();
	
	info->count = sem->count;
	info->maxCount = sem->maxCount;
	info->taskCount = tEventWaitCount(&sem->event);
	
	tTaskExitCritical(status);
}
	
uint32_t tSemDestory(tSem * sem)
{
	uint32_t status = tTaskEnterCritical();
	
	uint32_t count = tEventRemoveAll(&sem->event, (void *)0, tErrorDestory);
	
	sem->count =0;
	
	tTaskExitCritical(status);
	
	if(count)
	{
		tTaskSchedule();
	}
	
	return count;
}
