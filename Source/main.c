#include "tinyOS.h"
#define NVIC_INT_CTRL         0xE000Ed04    //�ж�״̬�����ƼĴ���
#define NVIC_PENDSET          0x10000000
#define NVIC_SYSPRI14         0xE000ED22    //PendSV���ȼ����üĴ���
#define NVIC_PENDSV_PRI       0x000000FF

#define  MEM32(addr)          *(volatile unsigned long *)(addr)
#define  MEM8(addr)           *(volatile unsigned char *)(addr)
	
void triggerPendSVC(void)
{
	MEM8(NVIC_SYSPRI14) = NVIC_PENDSV_PRI;  //����PendSVC�ж����ȼ�Ϊ��� 255
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSET;    //PendSVC���𣬽���PendSVC�жϷ������
}

void tTaskInit(tTask * task, void (*entry), void * param, tTaskStack * stack)
{
	task->stack = stack;
}


void delay(int count)
{
	while(--count > 0);
}

tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

void task1(void * param)
{
	for(;;)
	{}
}

void task2(void * param)
{
	for(;;)
	{}
}


int main()
{
	tTaskInit(&tTask1, task1, (void *)0x11111111, &task1Env[1024]);
	tTaskInit(&tTask2, task2, (void *)0x22222222, &task2Env[1024]);
}
