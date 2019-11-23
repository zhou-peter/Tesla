#include "common.h"
#include "kernel.h"
#include "soft_timer.h"
#include "communication_manager.h"
#include "accelerometer_manager.h"


GenericList_t list;

void KERNEL_Init(QueueHandle_t queue) {
	list.maxCount = 100;
	list.itemSize = sizeof(AccelData_t);

}

void KERNEL_AddToList(AccelData_t* newData){
	addToList(&list, newData);
}

void KERNEL_Task() {
	AccelData_t a;

	a.x = 1;
	a.y = 1;
	a.z = 1;

	KERNEL_AddToList(&a);

		a.x = 2;
		a.y = 2;
		a.z = 2;

		KERNEL_AddToList(&a);

	while (TRUE) {

		//every time send something

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
		osDelay(1);
	}
}

