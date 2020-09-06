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
	
	//�����쳣��ʱ����xPSR PC(R15) LR(R14) R12, R3-R0��ֵ�Զ�ѹ��currentTask�Ķ�ջ(PSP)
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0, PSP                       //��PSP�Ĵ�����ֵ���浽R0��   MRS: ״̬�Ĵ�����ͨ�üĴ����Ĵ���ָ�
	CBZ R0, PendSV_Handler_nosave     //�ж�R0��ֵ�Ƿ�Ϊ0���ǵĻ���ת��PendSV_Handler_nosave
	
	STMDB R0, {R4-R11}                //R0���ڵ���PSP��Ҳ����currentTask�Ķ�ջջ��ָ�룬��R4-R11���浽currentTask�Ķ�ջ
	LDR R1, =currentTask              //��currentTask�ĵ�ַ��R1
	LDR R1, [R1]                      //��R1��ȡ��currentTask��ָ���task�ĵ�ַ,task�ĵ�ַ��stack�ĵ�ַ��ͬ
	STR R0, [R1]                      //��R0���ڵ�ֵ(ջ��ָ��)д�뵽R1�ĵ�ַ����Ӧ���ڴ浥Ԫ��Ҳ����task��stackָ��(ָ���ջ)
	
PendSV_Handler_nosave
	LDR R0, = currentTask             //��currentTask�ĵ�ַ�ŵ�R0
	LDR R1, = nextTask                //��nextTask�ĵ�ַ�ŵ�R1
	LDR R2, [R1]                      //���洢����ַΪR1�������ݶ���Ĵ���R2,Ҳ����nextTask��ָ���task�ĵ�ַ
	STR R2, [R0]                      //��R2�Ĵ�����ֵд�뵽currentTask�����ַ��Ҳ����currentTask����ָ���task��֮ǰnextTask��ָ���task
	
	LDR R0, [R2]                      //��currentTask��ȡ������ָ���task���������ָ���stack��ַ
	LDMIA R0!, {R4-R11}               //��currentTask��task��stack�ﱣ���R4-R11��ֵ�������Ĵ���R4-R11
	
	MSR PSP, R0                       //��R0���ڵ�ֵ��Ҳ����ջ��ָ�룬д�뵽PSP  MSR: ͨ�üĴ�����״̬�Ĵ����Ĵ���ָ�
	ORR LR, LR, #0x04                 //�����˳�ʱʹ��PSP��Ϊʹ�õĶ�ջ����ΪĬ��ʹ��MSP
	
	BX LR                             //�˳��쳣��ʱ����PSP��ջ�����ݵ������⼸���Ĵ�����xPSR PC(R15) LR(R14) R12, R3-R0
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
