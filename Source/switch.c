#include "tinyOS.h"
#include "ARMCM3.h"

#define NVIC_INT_CTRL         0xE000Ed04    //中断状态及控制寄存器
#define NVIC_PENDSET          0x10000000
#define NVIC_SYSPRI14         0xE000ED22    //PendSV优先级配置寄存器
#define NVIC_PENDSV_PRI       0x000000FF

#define  MEM32(addr)          *(volatile unsigned long *)(addr)
#define  MEM8(addr)           *(volatile unsigned char *)(addr)
	
extern int scheduleLockCount;
extern tTask * taskTable[TINYOS_PRO_COUNT];
extern tBitmap taskProBitmap;
extern tList taskDelayList;
	
__asm void PendSV_Handler(void)
{
	/*在进入中断时编译器自动保存xPSR PC(R15) LR(R14) R12, R3-R0到PSP栈顶指针所指向的存储空间*/
    
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0, PSP                          // 把PSP的值放到R0
	CBZ R0, PendSV_Handler_nosave        // 判断R0的值是否为0，如果为0就是第一个任务，不用保护现场
	
	/***这一部分用来保存当前任务的寄存器R4-R11***/
	STMDB R0!, {R4-R11}                  // 把R4-R11的指压入PSP所指向的堆栈空间,压完后R0的指指向栈顶，值已改变
	
    /***把压栈完成后的栈顶指针放到currentTask的task->stack里****/
	LDR R1, =currentTask                 // 把currentTask的地址放到R1               
	LDR R1, [R1]                         // 把currentTask的地址存放的内容放到R1,也就是currentTask所指向的task的地址，也就是该task结构体的stack地址
	STR R0, [R1]                         // 把R0的指(当前栈顶指针)放入到R1存放的地址所指向的内存空间，也就是currentTask的task->stack指向的堆栈空间
	
PendSV_Handler_nosave
	
	/****这一部分用来将currentTask切换到nextTask****/
	LDR R0, =currentTask                 // 把currentTask的地址放到R0
	LDR R1, =nextTask                    // 把nextTask的地址放到R1
	LDR R1, [R1]                         // 从R1存放的地址取出nextTask所指向的task的地址，放到R1
	STR R1, [R0]                         // 把R1存放的task的地址放到currentTask地址的内存空间
	
	/****这一部分用来弹出堆栈空间保存的R4-R11的指到相应的寄存器****/
	LDR R0, [R1]                         // 把R1(存放的是currentTask所指向的task的地址)地址所指向内容(堆栈指针)放到R0
    LDMIA R0!, {R4-R11}                  // 把R0所指向的堆栈空间的内容弹出到R4-R11,完成后堆栈指针改变
 
	/****这一部分用来保存弹栈后改变的栈顶指针****/
	MSR PSP, R0                          //把R0现在的值，也就是栈顶指针，写入到PSP  MSR: 通用寄存器到状态寄存器的传送指令。
	ORR LR, LR, #0x04                    //设置退出时使用PSP作为使用的堆栈，因为默认使用MSP

	BX LR                                //退出异常的时候会把PSP堆栈的内容弹出到这几个寄存器：xPSR PC(R15) LR(R14) R12, R3-R0

}

void tTaskRunFirst()
{
	__set_PSP(0); //来自ARMCM3.h
	
    MEM8(NVIC_SYSPRI14) = NVIC_PENDSV_PRI;  //设置PendSVC中断优先级为最低 255
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSET;    //PendSVC悬起，进入PendSVC中断服务程序
}

void tTaskSwitch()
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSET;    //PendSVC悬起，进入PendSVC中断服务程序
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

/************时间片轮转**************/
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


/************延时触发切换**************/
void tTaskDelay(uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	currentTask->delayTicks = delay * 0.1;
	tBitmapClear(&taskProBitmap, currentTask->prio);
	tTaskExitCritical(status);
	
	tTaskSchedule();
}
/***************End********************/


/**************临界区****************/
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


/**************调度锁****************/
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

/**************就绪列表****************/
tTask * tTaskHighestReady()
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskProBitmap);
	return taskTable[highestPrio];
}

/**************延时队列****************/
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
