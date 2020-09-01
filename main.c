#define NVIC_INT_CTRL         0xE000Ed04    //中断状态及控制寄存器
#define NVIC_PENDSET          0x10000000
#define NVIC_SYSPRI14         0xE000ED22    //PendSV优先级配置寄存器
#define NVIC_PENDSV_PRI       0x000000FF

#define  MEM32(addr)          *(volatile unsigned long *)(addr)
#define  MEM8(addr)           *(volatile unsigned char *)(addr)
	
void triggerPendSVC(void)
{
	MEM8(NVIC_SYSPRI14) = NVIC_PENDSV_PRI;  //设置PendSVC中断优先级为最低 255
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSET;    //PendSVC悬起，进入PendSVC中断服务程序
}

typedef struct _BlockType_t
{
	unsigned long * stackPtr;
	unsigned long * stackEndPtr;
}BlockType_t;

BlockType_t * blockPtr;
BlockType_t ** blockPtrToPtr;


void delay(int count)
{
	while(--count > 0);
}

int flag;
unsigned long stackBuffer[8];
BlockType_t block;

int main()
{
	// blockPtr -> block -> stackPtr -> stackBuffer[1024]
	block.stackEndPtr = &stackBuffer[0];
	block.stackPtr = &stackBuffer[8];  //&stackBuffer[1024]是这个数组的最后一个元素stackBuffer[1023]地址的下一个地址
	blockPtr = &block;
	blockPtrToPtr = &blockPtr; //用以查看blockPtr的地址
	for(;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
		
		triggerPendSVC();
	}
}
