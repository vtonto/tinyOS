#ifndef TFLAGGROUP_H
#define TFLAGGROUP_H

#include "tEvent.h"

typedef struct _tFlagGroup
{
	tEvent event;
	uint32_t flag;
}tFlagGroup;

typedef struct _tFlagGroupInfo
{
	uint32_t flags;
	uint32_t taskCount;
}tFlagGroupInfo;
	

#define TFLAGGROUP_CLR     (0x00 << 0)
#define TFLAGGROUP_SET     (0x01 << 0)
#define TFLAGGROUP_ANY     (0x00 << 1)
#define TFLAGGROUP_ALL     (0x01 << 1)

#define TFLAGGROUP_SET_ALL  (TFLAGGROUP_SET | TFLAGGROUP_ALL)
#define TFLAGGROUP_SET_ANY  (TFLAGGROUP_SET | TFLAGGROUP_ANY)
#define TFLAGGROUP_CLR_ALL  (TFLAGGROUP_CLR | TFLAGGROUP_ALL)
#define TFLAGGROUP_CLR_ANY  (TFLAGGROUP_CLR | TFLAGGROUP_ANY)

#define TFLAGGROUP_CONSUME  (1 << 7)

void tFlagGroupInit(tFlagGroup * flagGroup, uint32_t flags);
uint32_t tFlagGroupWait(tFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t *resultFlag, uint32_t waitTicks);
uint32_t tFlagGroupNoWaitGet(tFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t *resultFlag);
void tFlagGroupNotify(tFlagGroup * flagGroup, uint8_t isSet, uint32_t flag);
void tFlagGroupGetInfo(tFlagGroup * flagGroup, tFlagGroupInfo * info);
uint32_t tFlagGroupDestory(tFlagGroup * flagGroup);
#endif
