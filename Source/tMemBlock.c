#include "tinyOS.h"

void tMemBlockInit (tMemBlock * memblock, uint8_t * memStart, uint32_t blockSize,uint32_t blockCnt)
{
	uint8_t * memBlockStart = (uint8_t *) memStart;
	uint8_t * memBlockEnd = memBlockStart +blockSize * blockCnt;
	
	if (blockSize < sizeof (tNode))
	{
		return;
	}
	
	tEventInit(&memblock->event, tEventTypeMemBlock);
	memblock->memStart = memStart;
	memblock->blockSize = blockSize;
	memblock->maxCount = blockCnt;
	
	tListInit(&memblock->blockList);
	while (memBlockStart< memBlockEnd)
	{
		tNodeInit((tNode *) memBlockStart);
		tListAddLast(&memblock->blockList, (tNode *)memBlockStart);
		
		memBlockStart += blockSize;
	}
}

uint32_t tMemBlockWait (tMemBlock * memBlock, uint8_t **mem, uint32_t waitTicks)
{
	uint32_t status = tTaskEnterCritical();
	
	if (tListCount(&memBlock->blockList) > 0)
	{
		*mem = (uint8_t *)tListRemoveFirst(&memBlock->blockList);
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else 
	{
		tEventWait (&memBlock->event, currentTask, (void *)0, tEventTypeMemBlock, waitTicks);
		tTaskExitCritical(status);
		
		tTaskSchedule();
		*mem = currentTask->waitingMsg;
		return currentTask->eventWaitingResult;
	}
}

uint32_t tMemBlockNoWaitGet (tMemBlock * memBlock, void ** mem)
{
uint32_t status = tTaskEnterCritical();
	
	if (tListCount(&memBlock->blockList) > 0)
	{
		*mem = (uint8_t *)tListRemoveFirst(&memBlock->blockList);
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else 
	{
		tTaskExitCritical(status);
		return tErrorResourceUnavaliable;
	}
}

void tMemBlockNotify (tMemBlock * memBlock, uint8_t *mem)
{
	uint32_t status = tTaskEnterCritical();
	
	if (tEventWaitCount(&memBlock->event)>0)
	{
		tTask * task = tEventWake(&memBlock->event, (void *)mem, tErrorNoError);
		if (task->prio < currentTask->prio)
		{
			tTaskSchedule();
		}
	}
	else
	{
		tListAddLast(&memBlock->blockList, (tNode *)mem);
	}
	tTaskExitCritical(status);
}

void tMemBlockGetInfo(tMemBlock * block, tMemBlockInfo * info)
{
	uint32_t status = tTaskEnterCritical();
	
	info->count = tListCount(&block->blockList);
	info->maxCount = block->maxCount;
	info->blockSize = block->blockSize;
	info->taskCount = tEventWaitCount(&block->event);
	
	tTaskExitCritical(status);
}

uint32_t tMemBlockDestory(tMemBlock * block)
{
	uint32_t status = tTaskEnterCritical();
	
	uint32_t count = tEventRemoveAll(&block->event, (void *)0, tErrorDelete);
	
	tTaskExitCritical(status);	
	
	if(count >0)
	{
		tTaskSchedule();
	}
	
	return count;
}

