#include "tinyOS.h"
#include "ARMCM3.h"

#define NVIC_INT_CTRL         0xE000Ed04    //中断状态及控制寄存器
#define NVIC_PENDSET          0x10000000
#define NVIC_SYSPRI14         0xE000ED22    //PendSV优先级配置寄存器
#define NVIC_PENDSV_PRI       0x000000FF

#define  MEM32(addr)          *(volatile unsigned long *)(addr)
#define  MEM8(addr)           *(volatile unsigned char *)(addr)
	
__asm void PendSV_Handler(void)
{
	IMPORT blockPtr
	
	LDR R0, = blockPtr //把blockPtr的地址加载到R0寄存器，现在R0存放的是blockPtr的地址
	LDR R0, [R0]       //把blockPt这个地址存放的block结构体的地址加载到R0,现在R0存放的是block结构体的地址，也就是stackPtr的地址
	LDR R0, [R0]       //把stackPtr存放的四个字节内容(stackBuffer[8]的地址)加载到R0
	
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
	__set_PSP(0); //来自ARMCM3.h
	
    MEM8(NVIC_SYSPRI14) = NVIC_PENDSV_PRI;  //设置PendSVC中断优先级为最低 255
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSET;    //PendSVC悬起，进入PendSVC中断服务程序
}

void tTaskSwitch()
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSET;    //PendSVC悬起，进入PendSVC中断服务程序
}
