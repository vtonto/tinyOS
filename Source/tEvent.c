#include "tinyOS.h"

void tEventInit(tEvent * event, tEventType type)
{
	tListInit(&event->waitingList);
	event->type = tEventTypeUnKnow;
}

void tEventWait(tEvent * event, tTask * task, void * msg, uint32_t state, uint32_t timeout)
{
	uint32_t status = tTaskEnterCritical();
	
	task->state |= state;
	task->waitingEvent = event;
	task->waitingMsg = msg;
	task->eventWaitingResult = tErrorNoError;
	
	tTaskReadyRemove(task);
	
	tListAddLast(&task->waitingEvent->waitingList, &task->linkNode);
	
	if(timeout)
	{
		tTimeTaskWait(task, timeout);
	}
	
	tTaskExitCritical(status);
}

tTask * tEventWake(tEvent * event, void * msg, uint32_t result)
{
	tNode * node;
	tTask * task = (tTask *)0;
	
	uint32_t status = tTaskEnterCritical();
	
	if((node = tListRemoveFirst(&event->waitingList)) != (tNode *)0)
	{
		task = tNodeParent(node, tTask, linkNode);
		task->waitingEvent = (tEvent *)0;
		task->waitingMsg = msg;
		task->eventWaitingResult = result;
		task->state &= ~TINYOS_TASK_WAIT_MASK;
		
		if(task->delayTicks !=0)
		{
			tTimeTaskRemove(task);
		}
		
		tTaskScheduleReady(task);
	}
	
	tTaskExitCritical(status);	
	
	return task;
}

void tEventRemoveTask(tTask * task, void * msg, uint32_t result)
{
	uint32_t status = tTaskEnterCritical();
	
	tListRemoveNode(&task->waitingEvent->waitingList, &task->linkNode);
	task->waitingEvent = (tEvent *)0;
	task->waitingMsg = msg;
	task->eventWaitingResult = result;
	task->state &= ~TINYOS_TASK_WAIT_MASK;
	
	tTaskExitCritical(status);
}
