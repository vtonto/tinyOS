#include "tinyOS.h"

tTask * currentTask;
tTask * nextTask;
tTask * taskTable[2];

void tTaskInit(tTask * task, void (*entry), void * param, tTaskStack * stack)
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
}

void tTaskSched()
{
	if(currentTask == taskTable[0])
		nextTask = taskTable[1];
	else
		nextTask = taskTable[0];
	
	tTaskSwitch();
}


void delay(int count)
{
	while(--count > 0);
}

tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

int task1Flag;

void task1Entry(void * param)
{
	unsigned long value = *(unsigned long *)param;
	value++;
	for(;;)
	{
		task1Flag =0;
		delay(100);
		task1Flag =1;
		delay(100);
		tTaskSched();
	}
}

int task2Flag;

void task2Entry(void * param)
{
	unsigned long value = *(unsigned long *)param;
	value++;
	for(;;)
	{
		task2Flag =0;
		delay(100);
		task2Flag =1;
		delay(100);
		tTaskSched();
	}
}


int main()
{
	tTaskInit(&tTask1, task1Entry, (void *)0x11111111, &task1Env[1024]);
	tTaskInit(&tTask2, task2Entry, (void *)0x22222222, &task2Env[1024]);
	
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	
	nextTask = taskTable[0];
	
	tTaskRunFirst();
	
	return 0;
}
