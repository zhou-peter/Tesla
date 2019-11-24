/*
 * circle_buffer.h
 *
 *  Created on: Nov 23, 2019
 *      Author: User
 */
#include "common.h"

#ifndef CIRCLE_BUFFER_H_
#define CIRCLE_BUFFER_H_

//buffer size must be aligned to item size
typedef struct
{
	u16 firstIndex:10;//byte offset
	u16 lastIndex:10;
    u16 itemsCount:10;
    u8	itemSize:8;	//in bytes

    u16 maxCount:10;
    u16 bufferSize:16;

    volatile u8* buffer;
} CircleBuffer_t;



extern void initCircleBuffer(CircleBuffer_t* instance, volatile u8* buffer,
		u16 bufferSize, u16 itemSize);


extern void addItem(CircleBuffer_t* instance, void* newItem);

extern void* getItem(CircleBuffer_t* instance, u16 index);
#endif /* CIRCLE_BUFFER_H_ */
