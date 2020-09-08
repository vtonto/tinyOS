#include "tinyOS.h"
#include "ARMCM3.h"

tTask * currentTask;
tTask * nextTask;
tTask * taskTable[2];


tTask tTask1;
tTask tTask2;

tTaskStack task1Stack[1024];
tTaskStack task2Stack[1024];

int task1Flag;
int task2Flag;

tTask tTaskIdle;
tTaskStack taskIdleStack[1024];
tTask * idleTask;

int scheduleLockCount;

int main()
{
	tSetSysTickPeriod(10);
	
	tTaskInit(&tTask1, task1Entry, (void *)0x11111111, &task1Stack[1024]);
	tTaskInit(&tTask2, task2Entry, (void *)0x22222222, &task2Stack[1024]);
	
	tTaskInit(&tTaskIdle, taskIdleEntry, (void *)0x33333333, &taskIdleStack[1024]);
	
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	idleTask = &tTaskIdle;
	
	nextTask = taskTable[0];
	
	tTaskRunFirst();
	
	return 0;
}

void tTaskScheduleLockInit()
{
	scheduleLockCount =0;
}

void tScheduleLockEnable()
{
	uint32_t status = tTaskEnterCritical();
	if(scheduleLockCount < 255)
	{
	    scheduleLockCount++;
	}
	tTaskExitCritical(status);
}

void tScheduleLockDisable()
{
	uint32_t status = tTaskEnterCritical();
	if(scheduleLockCount >0)
	{
		scheduleLockCount--;
	}
	tTaskExitCritical(status);
}

void tSetSysTickPeriod(uint32_t ms)
{
	SysTick->LOAD = ms * SystemCoreClock /1000 -1;              // 设置重载寄存器
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
	int i;
	uint32_t status = tTaskEnterCritical();
	for(i=0; i<2; i++)
	{
		if(taskTable[i]->delayTicks >0 )
		    taskTable[i]->delayTicks--;
	}
	tTaskExitCritical(status);
	tTaskSchedule();
}

void tTaskDelay(uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	currentTask->delayTicks = delay * 0.1;
	tTaskExitCritical(status);
	
	tTaskSchedule();
}

void tTaskSchedule()
{
	uint32_t status = tTaskEnterCritical();
	if(scheduleLockCount >0)
	{
	    tTaskExitCritical(status);
		return;	
	}
	if(currentTask == idleTask)
	{
		if(taskTable[0]->delayTicks ==0)
		{
			nextTask = taskTable[0];
		}
		else if(taskTable[1]->delayTicks ==0)
		{
			nextTask = taskTable[1];
		}
		else
		{
			tTaskExitCritical(status);
			return;
		}
	}
	else if(currentTask == taskTable[0])
	{
		if(taskTable[1]->delayTicks ==0)
		{
			nextTask = taskTable[1];
		}
		else if(taskTable[0]->delayTicks !=0)
		{
			nextTask = idleTask;
		}
		else
		{
			tTaskExitCritical(status);
			return;
		}
	}
	else
	{
		if(taskTable[0]->delayTicks ==0)
		{
			nextTask = taskTable[1];
		}
		else if(taskTable[1]->delayTicks !=0)
		{
			nextTask = idleTask;
		}
		else
		{
			tTaskExitCritical(status);
			return;
		}
	}
	
	tTaskExitCritical(status);
	
	tTaskSwitch();
}

void tTaskInit(tTask * task, void (*entry), void * param, tTaskStack * stack)
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
	task->delayTicks =0;
}

void task1Entry(void * param)
{
	for(;;)
	{
		tScheduleLockDisable();
		task1Flag =1;
		tTaskDelay(10);	
		tScheduleLockEnable();
		
		task1Flag =0;
		tTaskDelay(10);
	}
}

void task2Entry(void * param)
{
	for(;;)
	{
		task2Flag =1;
		tTaskDelay(10);
		task2Flag =0;
		tTaskDelay(10);
	}
}

void taskIdleEntry()
{
	for(;;)
	{}
}
