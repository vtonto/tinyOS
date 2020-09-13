#include "tinyOS.h"
#include "ARMCM3.h"

void tSetSysTickPeriod(uint32_t ms)
{
	SysTick->LOAD = ms * SystemCoreClock /1000 -1;              // …Ë÷√÷ÿ‘ÿºƒ¥Ê∆˜
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS)-1);
	SysTick->VAL =0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
	                SysTick_CTRL_TICKINT_Msk |
	                SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler()
{
	tTaskSystemTickHandler();
}

void tTaskSystemTickHandler(void)
{
	tNode * node = taskDelayList.firstNode;
	
	uint32_t status = tTaskEnterCritical();
	
	if(node != &(taskDelayList.headNode))
	{
		tTask * task = tNodeParent(node, tTask, delayNode);
		tNode * afterNode = taskDelayList.firstNode->nextNode;
		if(--task->remainTicks == 0)
		{
			tTimeTaskWake(task);
			tTaskScheduleReady(task);
			
			tNode * readyNode;
			tTask * readyTask;
			for(readyNode = afterNode; readyNode != &(taskDelayList.headNode); readyNode=readyNode->nextNode)
			{
				readyTask = tNodeParent(readyNode, tTask, delayNode);
				if(readyTask->remainTicks == 0)
				{
					tTimeTaskWake(readyTask);
			        tTaskScheduleReady(readyTask);
				}
				else
				{
					break;
				}
			}
		}
	}
	
	if(--currentTask->slice ==0)
	{
		if(tListCount(&(taskTable[currentTask->prio])) >0)
		{
			tListRemoveFirst(&taskTable[currentTask->prio]);
			tListAddLast(&taskTable[currentTask->prio], &(currentTask->linkNode));
			currentTask->slice = TINYOS_SLICE_MAX;
		}
	}

	tTaskExitCritical(status);
	tTaskSchedule();
}
