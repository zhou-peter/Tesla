#include "common.h"


#ifndef GENERIC_LIST_H_
#define GENERIC_LIST_H_


typedef struct
{
    // Any data type can be stored in this node
    void* data;
    struct Node* next;
} ListNode_t;


typedef struct
{
    u16 count:12;
    u16 maxCount:12;
    u8	itemSize:8;
    ListNode_t* head;
    ListNode_t* tail;
} GenericList_t;


extern void addToList(GenericList_t* list, void* newItem);


#endif /* GENERIC_LIST_H_ */
