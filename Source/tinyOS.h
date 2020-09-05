#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>

typedef uint32_t tTaskStack;

typedef struct _tTask
{
	tTaskStack * stack;
}tTask;

extern tTask * currentTask;
extern tTask * nexttTask;

void tTaskRunFirst(void);
void tTaskSched(void);
void tTaskSwitch(void);

#endif

