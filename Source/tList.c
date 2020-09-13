#include "tLib.h"

void tNodeInit(tNode * node)
{
	node->nextNode = node;
	node->preNode = node;
}

void tListInit(tList * list)
{
	list->firstNode = &(list->headNode);
	list->lastNode = &(list->headNode);
	list->nodeCount =0;
}

uint32_t tListCount(tList * list)
{
	return list->nodeCount;
}

tNode * tListFirstNode(tList * list)
{
	tNode * node = (tNode *)0;
	if(list->nodeCount !=0)
	{
		node = list->firstNode;
	}
	return node;
}

tNode * tListLastNode(tList * list)
{
	tNode * node = (tNode *)0;
	if(list->nodeCount !=0)
	{
		node = list->lastNode;
	}
	return node;
}

tNode * tListPreNode(tNode * node)
{
	if(node->preNode == node)
	{
		return (tNode *)0;
	}
	else
		return node->preNode;
}

tNode * tListNextNode(tNode * node)
{
	if(node->nextNode == node)
	{
		return (tNode *)0;
	}
	else
	{
		return node->nextNode;
	}
}

void tListRemoveAll(tList * list)
{
	uint32_t count;
	tNode * nextNode;
	
	nextNode = list->firstNode;
	for(count = tListCount(list); count>0; count--)
	{
		tNode * currentNode = nextNode;
		currentNode->nextNode = nextNode;
		currentNode->preNode = nextNode;
		
		nextNode = nextNode->nextNode;
	}
	
	tListInit(list);
}

tNode * tListRemoveFirst(tList * list)
{
	tNode * node = (tNode *)0;
	if(list->nodeCount !=0)
	{
		node = list->firstNode;
		
		node->nextNode->preNode = &(list->headNode);
		list->headNode.nextNode = node->nextNode;
		
		list->nodeCount--;
	}
	return node;
}

void tListRemoveNode(tList * list, tNode * node)
{
	node->preNode->nextNode = node->nextNode;
	node->nextNode->preNode = node->preNode;
	
	node->nextNode = node;
	node->preNode = node;
	
	list->nodeCount--;
}

void tListAddFirst(tList * list, tNode * node)
{
	node->nextNode = list->firstNode;
	node->preNode = &(list->headNode);
	
	list->firstNode->preNode = node;
	list->headNode.nextNode = node;
	
	list->nodeCount++;
}

void tListAddLast(tList * list, tNode * node)
{
	node->nextNode = &(list->headNode);
	node->preNode = list->lastNode;
	
	list->lastNode->nextNode = node;
	list->lastNode = node;
	
	list->nodeCount++;
}

void tListInsertAfter(tList * list, tNode * nodeAfter, tNode * nodeToInsert)
{
	nodeToInsert->preNode = nodeAfter;
	nodeToInsert->nextNode = nodeAfter->nextNode;

	nodeAfter->nextNode->preNode = nodeToInsert;	
	nodeAfter->nextNode = nodeToInsert;
	
	list->nodeCount++;
}

void tListInsertBefore(tList * list, tNode * nodeBefore, tNode * nodeToInsert)
{
	nodeToInsert->preNode = nodeBefore->preNode;
	nodeToInsert->nextNode = nodeBefore;
	
	nodeBefore->preNode->nextNode = nodeToInsert;
	nodeBefore->preNode = nodeToInsert;
	
	list->nodeCount++;
}

