#include "tinyOS.h"

tTask tTask1;
tTask tTask2;
tTask tTask3;
tTask tTask4;

tTaskStack task1Stack[128];
tTaskStack task2Stack[128];
tTaskStack task3Stack[128];
tTaskStack task4Stack[128];


int task1Flag;
int task2Flag;
int task3Flag;
int task4Flag;

void tInitApp()
{
	tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Stack[128]);
	tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Stack[128]);
	tTaskInit(&tTask3, task3Entry, (void *)0x33333333, 0, &task3Stack[128]);
	tTaskInit(&tTask4, task4Entry, (void *)0x44444444, 1, &task4Stack[128]);
}

tMbox mbox1;
tMbox mbox2;
void * msgBuffer1[20];
void * msgBuffer2[20];
uint32_t msg[20];

void task1Entry(void * param)
{
	tMboxInit(&mbox1, (void *)msgBuffer1, 20);
	for(;;)
	{
		uint32_t i=0;
		for(i=0; i<20; i++)
		{
			msg[i]=i;
			tMboxNotify(&mbox1, &msg[i], tMboxNotifyNormal);
		}
		tTaskDelay(100);
		for(i=0; i<20; i++)
		{
			msg[i]=i;
			tMboxNotify(&mbox1, &msg[i], tMboxNotifyFront);
		}
		tTaskDelay(100);
		
		task1Flag =0;
		tTaskDelay(1);
		task1Flag =1;
		tTaskDelay(1);
	}
}

void task2Entry(void * param)
{
	for(;;)
	{
		void * msg_temp;
		
		uint32_t err = tMboxWait(&mbox1, &msg_temp, 0);
		if(err == tErrorNoError)
		{
			uint32_t value = *(uint32_t *)msg_temp;
			task2Flag = value;
			tTaskDelay(1);
		}
		
		task2Flag =0;
		tTaskDelay(1);	
		task2Flag =1;
		tTaskDelay(1);
	}
}

void task3Entry(void * param)
{
	tMboxInit(&mbox2, msgBuffer2, 20);
	for(;;)
	{
		void * msg;
		tMboxWait(&mbox2, &msg, 100);
		
		task3Flag =0;
		tTaskDelay(1);
		task3Flag =1;
		tTaskDelay(1);
	}
}

void task4Entry(void * param)
{
	for(;;)
    {
		task4Flag =0;
		tTaskDelay(1);
		task4Flag =1;
		tTaskDelay(1);
	}
}
