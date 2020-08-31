#define NVIC_INT_CTRL         0xE000Ed04    //�ж�״̬�����ƼĴ���
#define NVIC_PENDSET          0x10000000
#define NVIC_SYSPRI14         0xE000ED22    //PendSV���ȼ����üĴ���
#define NVIC_PENDSV_PRI       0x000000FF

#define MEM32(addr)           *(volatile unsigned long *)(addr)
#define  MEM8(addr)           *(volatile unsigned char *)(addr)
	
void triggerPendSVC(void)
{
	MEM8(NVIC_SYSPRI14) = NVIC_PENDSV_PRI;  //����PendSVC�ж����ȼ�Ϊ��� 255
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSET;    //PendSVC���𣬽���PendSVC�жϷ������
}

typedef struct _BlockType_t
{
	unsigned long * stackPtr;
}BlockType_t;

BlockType_t * blockPtr;


void delay(int count)
{
	while(--count > 0);
}

int flag;
unsigned long stackBuffer[1024];
BlockType_t block;

int main()
{
	// blockPtr -> block -> stackPtr -> stackBuffer[1024]
	block.stackPtr = &stackBuffer[1024];
	blockPtr = &block;
	for(;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
		
		triggerPendSVC();
	}
}
