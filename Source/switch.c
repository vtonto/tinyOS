#include "tinyOS.h"
#include "ARMCM3.h"

#define NVIC_INT_CTRL         0xE000Ed04    //�ж�״̬�����ƼĴ���
#define NVIC_PENDSET          0x10000000
#define NVIC_SYSPRI14         0xE000ED22    //PendSV���ȼ����üĴ���
#define NVIC_PENDSV_PRI       0x000000FF

#define  MEM32(addr)          *(volatile unsigned long *)(addr)
#define  MEM8(addr)           *(volatile unsigned char *)(addr)
	
extern int scheduleLockCount;
extern tTask * taskTable[TINYOS_PRO_COUNT];
extern tBitmap taskProBitmap;
extern tList taskDelayList;
	
__asm void PendSV_Handler(void)
{
	/*�ڽ����ж�ʱ�������Զ�����xPSR PC(R15) LR(R14) R12, R3-R0��PSPջ��ָ����ָ��Ĵ洢�ռ�*/
    
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0, PSP                          // ��PSP��ֵ�ŵ�R0
	CBZ R0, PendSV_Handler_nosave        // �ж�R0��ֵ�Ƿ�Ϊ0�����Ϊ0���ǵ�һ�����񣬲��ñ����ֳ�
	
	/***��һ�����������浱ǰ����ļĴ���R4-R11***/
	STMDB R0!, {R4-R11}                  // ��R4-R11��ָѹ��PSP��ָ��Ķ�ջ�ռ�,ѹ���R0��ָָ��ջ����ֵ�Ѹı�
	
    /***��ѹջ��ɺ��ջ��ָ��ŵ�currentTask��task->stack��****/
	LDR R1, =currentTask                 // ��currentTask�ĵ�ַ�ŵ�R1               
	LDR R1, [R1]                         // ��currentTask�ĵ�ַ��ŵ����ݷŵ�R1,Ҳ����currentTask��ָ���task�ĵ�ַ��Ҳ���Ǹ�task�ṹ���stack��ַ
	STR R0, [R1]                         // ��R0��ָ(��ǰջ��ָ��)���뵽R1��ŵĵ�ַ��ָ����ڴ�ռ䣬Ҳ����currentTask��task->stackָ��Ķ�ջ�ռ�
	
PendSV_Handler_nosave
	
	/****��һ����������currentTask�л���nextTask****/
	LDR R0, =currentTask                 // ��currentTask�ĵ�ַ�ŵ�R0
	LDR R1, =nextTask                    // ��nextTask�ĵ�ַ�ŵ�R1
	LDR R1, [R1]                         // ��R1��ŵĵ�ַȡ��nextTask��ָ���task�ĵ�ַ���ŵ�R1
	STR R1, [R0]                         // ��R1��ŵ�task�ĵ�ַ�ŵ�currentTask��ַ���ڴ�ռ�
	
	/****��һ��������������ջ�ռ䱣���R4-R11��ָ����Ӧ�ļĴ���****/
	LDR R0, [R1]                         // ��R1(��ŵ���currentTask��ָ���task�ĵ�ַ)��ַ��ָ������(��ջָ��)�ŵ�R0
    LDMIA R0!, {R4-R11}                  // ��R0��ָ��Ķ�ջ�ռ�����ݵ�����R4-R11,��ɺ��ջָ��ı�
 
	/****��һ�����������浯ջ��ı��ջ��ָ��****/
	MSR PSP, R0                          //��R0���ڵ�ֵ��Ҳ����ջ��ָ�룬д�뵽PSP  MSR: ͨ�üĴ�����״̬�Ĵ����Ĵ���ָ�
	ORR LR, LR, #0x04                    //�����˳�ʱʹ��PSP��Ϊʹ�õĶ�ջ����ΪĬ��ʹ��MSP

	BX LR                                //�˳��쳣��ʱ����PSP��ջ�����ݵ������⼸���Ĵ�����xPSR PC(R15) LR(R14) R12, R3-R0

}

void tTaskRunFirst()
{
	__set_PSP(0); //����ARMCM3.h
	
    MEM8(NVIC_SYSPRI14) = NVIC_PENDSV_PRI;  //����PendSVC�ж����ȼ�Ϊ��� 255
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSET;    //PendSVC���𣬽���PendSVC�жϷ������
}

void tTaskSwitch()
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSET;    //PendSVC���𣬽���PendSVC�жϷ������
}

void tTaskInit(tTask * task, void (*entry), void * param, uint32_t prio, tTaskStack * stack)
{
	/*******��һ���ֵļĴ�����Ӳ���Զ����浽MSP��PSP��**************/
	*(--stack) = (unsigned long)(1<<24);  // xPSR
	*(--stack) = (unsigned long)entry;    // R15 PC�Ĵ�����ֵ����Ϊ��ں���
	*(--stack) = (unsigned long)0x14;     // R14 LR
	*(--stack) = (unsigned long)0x12;     // R12 
	*(--stack) = (unsigned long)0x03;     // R3
	*(--stack) = (unsigned long)0x02;     // R2	
	*(--stack) = (unsigned long)0x01;     // R1
	*(--stack) = (unsigned long)param;    // R0  �����һ������

	/************��һ���ֵļĴ�����Ҫ�ֶ�����***********************/	
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
	
	task->prio = prio;
	tBitmapSet(&taskProBitmap, prio);
	taskTable[prio] = task;
	
	task->state = TINYOS_TASK_STATE_RDY;
	tNodeInit(&(task->delayNode));
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

/************ʱ��Ƭ��ת**************/
void tSetSysTickPeriod(uint32_t ms)
{
	SysTick->LOAD = ms * SystemCoreClock /1000 -1;              // �������ؼĴ���
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
	for(i=0; i<TINYOS_PRO_COUNT; i++)
	{
		if(taskTable[i]->delayTicks >0 )
		    taskTable[i]->delayTicks--;
		else
			tBitmapSet(&taskProBitmap, i);
	}
	tTaskExitCritical(status);
	tTaskSchedule();
}
/***************End********************/


/************��ʱ�����л�**************/
void tTaskDelay(uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	currentTask->delayTicks = delay * 0.1;
	tBitmapClear(&taskProBitmap, currentTask->prio);
	tTaskExitCritical(status);
	
	tTaskSchedule();
}
/***************End********************/


/**************�ٽ���****************/
uint32_t tTaskEnterCritical()
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

void tTaskExitCritical(uint32_t status)
{
	__set_PRIMASK(status);
}
/***************End********************/


/**************������****************/
void tTaskScheduleInit()
{
	tBitmapInit(&taskProBitmap);
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
/***************End********************/

/**************�����б�****************/
tTask * tTaskHighestReady()
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskProBitmap);
	return taskTable[highestPrio];
}

/**************��ʱ����****************/
void tTaskDelayListInit()
{
	tListInit(&taskDelayList);
}

void tTimeTaskWait(tTask * task, uint32_t ticks)
{
	task->delayTicks = ticks * 0.1;
	tListAddFirst(&taskDelayList, &(task->delayNode));
	task->state = TINYOS_TASK_STATE_DELAY;
}

void tTimeTaskWake(tTask * task)
{
	tListRemoveNode(&taskDelayList, &(task->delayNode));
	task->state &= ~TINYOS_TASK_STATE_DELAY;
}

void tTaskScheduleReady(tTask * task)
{
	tBitmapSet(&taskProBitmap, task->prio);
	taskTable[task->prio] = task;
	task->state = TINYOS_TASK_STATE_RDY;
}

void tTaskScheduleUnReady(tTask * task)
{
	tBitmapClear(&taskProBitmap, task->prio);
	taskTable[task->prio] = (tTask *)0;
	task->state &= ~TINYOS_TASK_STATE_RDY;
}
