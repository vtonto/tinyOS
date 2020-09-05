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
