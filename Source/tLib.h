#ifndef TLIB_H
#define TLIB_H
#include <stdint.h>

typedef struct
{
	uint32_t bitmap;
}tBitmap;

void tBitmapInit(tBitmap * Bitmap);
void tBitmapSet(tBitmap * Bitmap, uint32_t pos);
void tBitmapClear(tBitmap * Bitmap, uint32_t pos);
uint32_t tBitmapGetFirstSet(tBitmap * Bitmap);
uint32_t tBitmapPosCount(void);

typedef struct _tNode
{
	struct _tNode * preNode;
	struct _tNode * nextNode;
}tNode;

typedef struct _tList
{
	tNode headNode;
	uint32_t nodeCount;
}tList;

#define firstNode       headNode->nextNode
#define lastNode        headNode->preNode

void tNodeInit(tNode * node);
void tListInit(tList * list);
uint32_t tListCount(tList * list);
tNode * tListFirstNode(tList * list);
tNode * tListLastNode(tList * list);
tNode * tListPreNode(tList * list);
tNode * tListNextNode(tList * list);
void tListRemoveAll(tList * list);
tNode * tListRemoveFirst(tList * list);
void tListRemoveNode(tList * list, tNode * node);
void tListAddFirst(tList * list, tNode * node);
void tListAddLast(tList * list, tNode * node);
void tListInsertAfter(tList * list, tNode nodeAfter, tNode * nodeToInsert);
#endif
