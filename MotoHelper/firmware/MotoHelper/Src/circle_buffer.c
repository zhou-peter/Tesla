#include "circle_buffer.h"
#include "common.h"

void initCircleBuffer(CircleBuffer_t* instance, volatile u8* buffer,
		u16 bufferSize, u16 itemSize) {
	instance->buffer = buffer;
	instance->firstIndex = 0;
	instance->lastIndex = 0;
	instance->itemsCount = 0;
	instance->bufferSize = bufferSize;
	instance->itemSize = itemSize;
	instance->maxCount = bufferSize / itemSize;
}



void addItem(CircleBuffer_t* instance, void* newItem) {


	if (instance->itemsCount == 0) {
		instance->firstIndex = 0;
		instance->lastIndex = 0;
		instance->itemsCount=1;
		goto mem_copy;
	} else {
		//increase last index
		instance->lastIndex += instance->itemSize;
		//move to start
		if (instance->lastIndex >= instance->bufferSize - 1) {
			instance->lastIndex = 0;
		}
		if (instance->lastIndex==instance->firstIndex){
			//removing top item
			instance->firstIndex += instance->itemSize;
			if (instance->firstIndex >= instance->bufferSize - 1) {
				instance->firstIndex = 0;
			}
		}else{
			instance->itemsCount++;
		}
	}

mem_copy: ;
	// Copy contents of new_data to newly allocated memory.
	// Assumption: char takes 1 byte.
	int j;
	for (j = 0; j < instance->itemSize; j++) {
		*(char *) (instance->buffer + instance->lastIndex + j) =
				*(char *) (newItem + j);
	}

}


void* getItem(CircleBuffer_t* instance, u16 index)
{
	if (instance->firstIndex<=instance->lastIndex){
simple_get:
		return (void *) (instance->buffer + instance->firstIndex
				+ index * instance->itemSize);
	}
	else if (instance->firstIndex>instance->lastIndex)
	{
		if (instance->firstIndex + (index*instance->itemSize)
				>= instance->bufferSize - 1)
		{
			int tailCount = (instance->bufferSize-instance->firstIndex)/instance->itemSize;
			index-=tailCount;
			return (void *) (instance->buffer
							+ index * instance->itemSize);
		}
		else
		{
			goto simple_get;
		}
	}
}



