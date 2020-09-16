#ifndef TMBOX_H
#define TMBOX_H

#include "tEvent.h"

#define tMboxNotifyNormal 0
#define tMboxNotifyFront  1

typedef struct _tMbox
{
	tEvent event;
	uint32_t count;
	uint32_t read;
	uint32_t write;
	uint32_t maxCount;
	
	void ** msgBuffer;
}tMbox;

void tMboxInit(tMbox * mbox, void ** msgBuffer, uint32_t maxCount);
uint32_t tMboxWait(tMbox * mbox, void ** msg, uint32_t waitTicks);
uint32_t tMboxWaitNoGet(tMbox * mbox, void ** msg);
uint32_t tMboxNotify(tMbox * mbox, void *msg, uint32_t notifyOption);
void tMboxFlush(tMbox * mbox);
uint32_t tMboxDestory(tMbox * mbox);

#endif
