#include "common.h"
#include "generic_list.h"

/* add new item to list. if size excced, remove first element
 * it like a queue
 *  */
void addToList(GenericList_t* list, void* newItem) {

	//clear first alement
	if (list->count == list->maxCount) {
		struct ListNode_t* newHead = list->head->next;
		vPortFree(list->head->data);
		vPortFree(list->head);
		list->head = newHead;
	}

	// Allocate memory for node
	struct ListNode_t* newNode = (struct ListNode_t*) pvPortMalloc(
			sizeof(struct ListNode_t));
	newNode->data = pvPortMalloc(list->itemSize);


	if (list->head == NULL || list->tail == NULL) {
		list->head = newNode;
		list->head->next = newNode;
		list->tail = newNode;
	} else {
		list->tail->next=newNode;
		list->tail = newNode;
	}

	list->count++;

	// Copy contents of new_data to newly allocated memory.
	// Assumption: char takes 1 byte.
	u32 i;
	for (i=0; i<list->itemSize; i++)
	{
		*(char *)(newNode->data + i) = *(char *)(newItem + i);
	}

}

