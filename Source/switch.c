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
	IMPORT blockPtr
	
	LDR R0, = blockPtr //��blockPtr�ĵ�ַ���ص�R0�Ĵ���������R0��ŵ���blockPtr�ĵ�ַ
	LDR R0, [R0]       //��blockPt�����ַ��ŵ�block�ṹ��ĵ�ַ���ص�R0,����R0��ŵ���block�ṹ��ĵ�ַ��Ҳ����stackPtr�ĵ�ַ
	LDR R0, [R0]       //��stackPtr��ŵ��ĸ��ֽ�����(stackBuffer[8]�ĵ�ַ)���ص�R0
	
	STMDB R0!, {R4-R11}
	
	LDR R1, = blockPtr
	LDR R1, [R1]
	STR R0, [R1]
	
	ADD R4, R4, #1
	ADD R5, R5, #1
	
	LDMIA R0!, {R4-R11}
	
	BX LR 
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
