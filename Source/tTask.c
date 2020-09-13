#include "tinyOS.h"

void tSystemInit()
{
	tTaskDelayListInit();
	tBitmapInit(&taskProBitmap);
	tTaskScheduleInit();
	tTaskReadyTableInit();

}

void tTaskInit(tTask * task, void (*entry), void * param, uint32_t prio, tTaskStack * stack)
{
	/*******这一部分的寄存器是硬件自动保存到MSP或PSP的**************/
	*(--stack) = (unsigned long)(1<<24);  // xPSR
	*(--stack) = (unsigned long)entry;    // R15 PC寄存器的值设置为入口函数
	*(--stack) = (unsigned long)0x14;     // R14 LR
	*(--stack) = (unsigned long)0x12;     // R12 
	*(--stack) = (unsigned long)0x03;     // R3
	*(--stack) = (unsigned long)0x02;     // R2	
	*(--stack) = (unsigned long)0x01;     // R1
	*(--stack) = (unsigned long)param;    // R0  程序第一个参数

	/************这一部分的寄存器需要手动保存***********************/	
	*(--stack) = (unsigned long)0x11;     // R11	
	*(--stack) = (unsigned long)0x10;     // R10
    *(--stack) = (unsigned long)0x09;     // R09		
	*(--stack) = (unsigned long)0x08;     // R08
    *(--stack) = (unsigned long)0x07;     // R07
    *(--stack) = (unsigned long)0x06;     // R06
    *(--stack) = (unsigned long)0x05;     // R05
    *(--stack) = (unsigned long)0x04;     // R04
	
	task->stack = stack;
	task->delayTicks = 0;
	task->remainTicks = 0;
	
	task->prio = prio;
	tBitmapSet(&taskProBitmap, prio);
	
	task->state = TINYOS_TASK_STATE_RDY;
	tNodeInit(&(task->delayNode));
	
	task->slice = TINYOS_SLICE_MAX;
	tNodeInit(&(task->linkNode));
	tListAddFirst(&taskTable[task->prio], &(task->linkNode));
	
	task->suspendCount = 0;
	
}

void tTaskSchedule()
{
	tTask * tempTask;
	if(scheduleLockCount >0)
	{
		return;	
	}
	
	tempTask = tTaskHighestReady();
	if(currentTask != tempTask)
	{
		nextTask = tempTask;
		tTaskSwitch();
	}
}

void tTaskSuspend(tTask * task)
{
	uint32_t status = tTaskEnterCritical();
	
	if(!(task->state & TINYOS_TASK_STATE_SUSPEND))
	{
		if(++task->suspendCount <=1)
		{
			task->state = TINYOS_TASK_STATE_SUSPEND;
			tTaskScheduleUnReady(task);
			if(task == currentTask)
			{
				tTaskSchedule();
			}
		}
	}
	
	tTaskExitCritical(status);
}

void tTaskResume(tTask * task)
{
	uint32_t status = tTaskEnterCritical();
	
	if(--task->suspendCount ==0)
	{
		tTaskScheduleReady(task);
		task->state &= ~TINYOS_TASK_STATE_SUSPEND;
		tTaskSchedule();
	}
	
	tTaskExitCritical(status);
}
