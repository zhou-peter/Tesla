#include "common.h"
#include "kernel.h"
#include "soft_timer.h"
#include "circle_buffer.h"
#include "communication_manager.h"
#include "accelerometer_manager.h"

volatile u8 dataBuffer[sizeof(AccelData_t)*10];
CircleBuffer_t data;

void KERNEL_Init() {

	initCircleBuffer(&data, &dataBuffer, sizeof(dataBuffer), sizeof(AccelData_t));
}


//ПОТРЕБЛЕНИЕ листо 8 + 8 на один элемент
//1 секунда при 100Гц 16*100 = полтора килобайта
void KERNEL_AddToList(AccelData_t* newData){

}

void KERNEL_Task() {
	AccelData_t a;


int i=0;
for (i=0;i<13;i++){
	a.x = i;
	a.y = i;
	a.z = i;
	addItem(&data, &a);
}

for (i=0;i<data.itemsCount;i++){
	AccelData_t* d = (AccelData_t*)getItem(&data, i);
	if (d->x==0){
		d->x=1;
	}
}

	while (TRUE) {

		//every time send something
/*
		if (CommState.CommDriverReady == TRUE && CommState.TxState==TxIdle
				&& list.count > 0) {

			//send as much as possable
			taskENTER_CRITICAL();
			//available data size
			u16 itemsCount = list.count * list.itemSize;
			while (itemsCount>COMM_OUT_MAX_BODY_SIZE)
			{
				itemsCount-=list.itemSize;
			}
			//createOutPacketAndSend(0x11, itemsCount, &accelDataBuffer);
			taskEXIT_CRITICAL();
		}
		*/
		osDelay(1);
	}
}

