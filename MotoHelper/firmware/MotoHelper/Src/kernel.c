#include "common.h"
#include "kernel.h"
#include "soft_timer.h"
#include "circle_buffer.h"
#include "communication_manager.h"
#include "accelerometer_manager.h"

volatile u8 dataBuffer[sizeof(AccelData_t) * 10];
CircleBuffer_t data;

void KERNEL_Init() {
	initCircleBuffer(&data, &dataBuffer, sizeof(dataBuffer),
			sizeof(AccelData_t));
}

void KERNEL_Task() {

	while (TRUE) {

		//put new data to queue
		if (AccelState.State == AccelDataRetrived) {
			addItem(&data, &AccelData);
			AccelState.State = AccelShouldRequest;
		}

		//every time send something

		if (CommState.CommDriverReady == TRUE && CommState.TxState == TxIdle
				&& data.itemsCount > 0) {

			//send as much as possable

			//available data size
			u16 bytesCount = data.itemsCount * data.itemSize;
			while (bytesCount > COMM_OUT_MAX_BODY_SIZE) {
				bytesCount -= data.itemSize;
			}

			volatile u8* bodyPtr = &commOutBuf[COMM_OUT_BODY_OFFSET];

			int i = 0;
			for (i = data.itemsCount - 1; i >= 0; i--) {
				volatile u8* accelDataPtr = getItem(&data, i);
				copy(accelDataPtr, bodyPtr, data.itemSize);
				bodyPtr+=data.itemSize;
			}

			createOutPacketAndSend(0x11, bytesCount, NULL);

		}

		osDelay(1);
	}
}

/*
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
 */
