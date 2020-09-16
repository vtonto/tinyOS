#include "tinyOS.h"

void tMboxInit(tMbox * mbox, void ** msgBuffer, uint32_t maxCount)
{
	tEventInit(&mbox->event, tEventTypeMbox);
	
	mbox->count =0;
	mbox->read =0;
	mbox->write =0;
	mbox->maxCount = maxCount;
	mbox->msgBuffer = msgBuffer;
}

uint32_t tMboxWait(tMbox * mbox, void ** msg, uint32_t waitTicks)
{
	uint32_t status = tTaskEnterCritical();
	
	if(mbox->count)
	{
		--mbox->count;
		* msg = mbox->msgBuffer[mbox->read++];
		if(mbox->read >= mbox->maxCount)
		{
			mbox->read =0;
		}
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	
	else
	{
		tEventWait(&mbox->event, currentTask, (void *)0, tEventTypeMbox, waitTicks);
		tTaskSchedule();
		tTaskExitCritical(status);
		return currentTask->eventWaitingResult;
	}
}

uint32_t tMboxWaitNoGet(tMbox * mbox, void ** msg)
{
	uint32_t status = tTaskEnterCritical();
	
	if(mbox->count)
	{
		--mbox->count;
		* msg = mbox->msgBuffer[mbox->read++];
		if(mbox->read >= mbox->maxCount)
		{
			mbox->read =0;
		}
		tTaskExitCritical(status);
		return tErrorNoError;
	}
    else
	{
		tTaskExitCritical(status);
		return tErrorResourceUnavaliable;
	}		
}

uint32_t tMboxNotify(tMbox * mbox, void *msg, uint32_t notifyOption)
{
	uint32_t status = tTaskEnterCritical();
	
	if(tEventWaitCount(&mbox->event))
	{
		tTask * task = tEventWake(&mbox->event, (void *)0, tErrorNoError);
		if(task->prio < currentTask->prio)
		{
			tTaskSchedule();
		}
	}
	else
	{
		if(mbox->count >= mbox->maxCount)
		{
			tTaskExitCritical(status);
		    return tErrorResourceFull;
		}
		
		if(notifyOption & tMboxNotifyFront)
		{
			if(mbox->read <=0)
			{
				mbox->read = mbox->maxCount -1;
			}
			else
			{
				--mbox->read;
			}
			mbox->msgBuffer[mbox->read] = msg;
		}
		else
		{
			mbox->msgBuffer[mbox->write++] = msg;
			if(mbox->write >= mbox->maxCount)
			{
				mbox->write = 0;
			}
		}
		mbox->count++;
	}
	tTaskExitCritical(status);
	return tErrorNoError;
}

void tMboxFlush(tMbox * mbox)
{
	uint32_t status = tTaskEnterCritical();
	
	if(tEventWaitCount(&mbox->event) ==0)
	{
		mbox->count =0;
		mbox->read =0;
		mbox->write =0;
	}
	
	tTaskExitCritical(status);
}

uint32_t tMboxDestory(tMbox * mbox)
{
	uint32_t status = tTaskEnterCritical();
	
	uint32_t count = tEventRemoveAll(&mbox->event, (void *)0, tErrorDelete);
	
	tTaskExitCritical(status);
	
	if(count >0)
	{
		tTaskSchedule();
	}
	
	return count;
}
