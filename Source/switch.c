#include "tinyOS.h"
#include "ARMCM3.h"

#define NVIC_INT_CTRL         0xE000Ed04    //�ж�״̬�����ƼĴ���
#define NVIC_PENDSET          0x10000000
#define NVIC_SYSPRI14         0xE000ED22    //PendSV���ȼ����üĴ���
#define NVIC_PENDSV_PRI       0x000000FF

#define  MEM32(addr)          *(volatile unsigned long *)(addr)
#define  MEM8(addr)           *(volatile unsigned char *)(addr)
	
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
void tTaskReadyTableInit()
{
	int i;
	for(i=0; i < TINYOS_PRO_COUNT; i++)
	{
		tListInit(&taskTable[i]);
	}	
}

tTask * tTaskHighestReady()
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskProBitmap);
	tNode * node = tListFirstNode(&taskTable[highestPrio]);
	return tNodeParent(node, tTask, linkNode);
}


void tTaskScheduleReady(tTask * task)
{
	tBitmapSet(&taskProBitmap, task->prio);
	tListAddFirst(&taskTable[task->prio], &(task->linkNode));
	task->state = TINYOS_TASK_STATE_RDY;
}

void tTaskScheduleUnReady(tTask * task)
{
	tListRemoveNode(&taskTable[task->prio], &(task->linkNode));
	task->state &= ~TINYOS_TASK_STATE_RDY;
	if(tListCount(&taskTable[task->prio]) == 0)
	{
	    tBitmapClear(&taskProBitmap, task->prio);
	}
}
