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
	
	//进入异常的时候会把xPSR PC(R15) LR(R14) R12, R3-R0的值自动压入currentTask的堆栈(PSP)
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0, PSP                       //把PSP寄存器的值保存到R0里   MRS: 状态寄存器到通用寄存器的传送指令。
	CBZ R0, PendSV_Handler_nosave     //判断R0的值是否为0，是的话就转到PendSV_Handler_nosave
	
	STMDB R0, {R4-R11}                //R0现在等于PSP，也就是currentTask的堆栈栈顶指针，将R4-R11保存到currentTask的堆栈
	LDR R1, =currentTask              //把currentTask的地址给R1
	LDR R1, [R1]                      //从R1中取出currentTask所指向的task的地址,task的地址与stack的地址相同
	STR R0, [R1]                      //将R0现在的值(栈顶指针)写入到R1的地址所对应的内存单元，也就是task的stack指针(指向堆栈)
	
PendSV_Handler_nosave
	LDR R0, = currentTask             //将currentTask的地址放到R0
	LDR R1, = nextTask                //将nextTask的地址放到R1
	LDR R2, [R1]                      //将存储器地址为R1的字数据读入寄存器R2,也就是nextTask所指向的task的地址
	STR R2, [R0]                      //将R2寄存器的值写入到currentTask这个地址，也就是currentTask现在指向的task是之前nextTask所指向的task
	
	LDR R0, [R2]                      //从currentTask里取出它所指向的task这个任务所指向的stack地址
	LDMIA R0!, {R4-R11}               //将currentTask的task的stack里保存的R4-R11的值弹出到寄存器R4-R11
	
	MSR PSP, R0                       //把R0现在的值，也就是栈顶指针，写入到PSP  MSR: 通用寄存器到状态寄存器的传送指令。
	ORR LR, LR, #0x04                 //设置退出时使用PSP作为使用的堆栈，因为默认使用MSP
	
	BX LR                             //退出异常的时候会把PSP堆栈的内容弹出到这几个寄存器：xPSR PC(R15) LR(R14) R12, R3-R0
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
