#include "tinyOS.h"

void tTaskDelayListInit()
{
	tListInit(&taskDelayList);
}

void tTaskDelay(uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	//tTimeTaskWait(currentTask, delay);
    tTimeTaskWait(currentTask, delay);
	tTaskScheduleUnReady(currentTask);
	tTaskExitCritical(status);
	
	tTaskSchedule();
}

void tTimeTaskWait(tTask * task, uint32_t ticks)
{
	task->delayTicks = (uint32_t)(ticks * 0.1);
	if( taskDelayList.firstNode != &(taskDelayList.headNode) )
	{
		tTask * listFirstTask = tNodeParent(taskDelayList.firstNode, tTask, delayNode);
		if(task->delayTicks < listFirstTask->remainTicks)
		{
			tListAddFirst(&taskDelayList, &(task->delayNode));
			task->state = TINYOS_TASK_STATE_DELAY;
			task->remainTicks = task->delayTicks;
			listFirstTask->remainTicks -= task->remainTicks;		
			return;
		}
		else
		{
	        tNode * nextNode;
	        nextNode = taskDelayList.secondNode;
			if(taskDelayList.secondNode != &(taskDelayList.headNode))
			{
				task->remainTicks = task->delayTicks - listFirstTask->remainTicks;
			    uint8_t count;
	            for(count = tListCount(&taskDelayList)-1; count>0; count--)
	            {
					tNode * current_Node = nextNode;
					tTask * current_Task = tNodeParent(current_Node, tTask, delayNode);
		            if(task->remainTicks < current_Task->remainTicks)
				    {					
						tListInsertBefore(&taskDelayList, current_Node, &(task->delayNode));
                        current_Task->remainTicks -= task->remainTicks;
						return;
				    }
					if(count == 1)
					{
						tListInsertAfter(&taskDelayList, current_Node, &(task->delayNode));
                        task->remainTicks -= current_Task->remainTicks;						
						return;		
					}
					task->remainTicks -= current_Task->remainTicks;
					nextNode = nextNode->nextNode;
				}

	        } 
			else
			{
				tListInsertAfter(&taskDelayList, taskDelayList.firstNode, &(task->delayNode));
				task->remainTicks = task->delayTicks - listFirstTask->remainTicks;
				return;
		    }
	    }
	}
	else
	{
		tListAddFirst(&taskDelayList, &(task->delayNode));
		task->remainTicks = task->delayTicks;	
        task->state = TINYOS_TASK_STATE_DELAY;		
		return;
	}
}


void tTimeTaskWake(tTask * task)
{
	tListRemoveFirst(&taskDelayList);
	task->state &= ~TINYOS_TASK_STATE_DELAY;
}

void tTimeTaskRemove(tTask * task)
{
	tListRemoveNode(&taskDelayList, &(task->delayNode));
}
