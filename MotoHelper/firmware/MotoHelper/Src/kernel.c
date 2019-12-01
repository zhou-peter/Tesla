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

void sendAccelData()
{
	//send as much as possable
	//available data size
	u16 bodySize = data.itemsCount * data.itemSize;
	bodySize+=4;
	while (bodySize > COMM_OUT_MAX_BODY_SIZE) {
		bodySize -= data.itemSize;
	}

	volatile u8* bodyPtr = &commOutBuf[COMM_OUT_BODY_OFFSET];
	u32 ms = AccelState.ms;

	copy(&ms, bodyPtr, 4);
	bodyPtr+=4;

	int i = 0;
	for (i = data.itemsCount - 1; i >= 0; i--) {
		volatile u8* accelDataPtr = getItem(&data, i);
		//AccelData_t accelData = (AccelData_t)(*accelDataPtr);
		copy(accelDataPtr, bodyPtr, data.itemSize);
		bodyPtr+=data.itemSize;
	}

	createOutPacketAndSend(0x11, bodySize, NULL);
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

			sendAccelData();

			//createOutPacketAndSend(0x01, 0, NULL);
			osDelay(1000);
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
