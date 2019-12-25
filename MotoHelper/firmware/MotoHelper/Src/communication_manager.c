/*
 * communication.c
 *
 *  Created on: 30 окт. 2019 г.
 *      Author: User
 */
#include "communication_manager.h"
#include "soft_timer.h"

#define PACKET_START 0x7C

#define KEEP_ALIVE_PERIOD 10000

volatile u8 commInBuf[COMM_IN_BUF_SIZE];
volatile u8 commOutBuf[COMM_OUT_BUF_SIZE];
volatile OutPacket debugPacket;
volatile CommState_t CommState;
volatile RxStates stateBeforeTimeout;
volatile u32 lastTimeStampReset = 0;
TaskHandle_t commHandle;

void COMM_Init(TaskHandle_t taskHandle) {
	commHandle = taskHandle;
}

void COMM_ResumeTaskFromISR() {
	BaseType_t false = pdFALSE;
	vTaskNotifyGiveFromISR(commHandle, &false);
	taskYIELD();
}

void COMM_PeriodElapsedCallback() {
	stateBeforeTimeout = CommState.RxState;
	switch (CommState.RxState) {
	case ReceivingSize:
	case ReceivingPacket:
		CommState.RxState = ReceivingTimeout;
		break;
	default:
		break;
	}
	BaseType_t false = pdFALSE;
	vTaskNotifyGiveFromISR(commHandle, &false);
	taskYIELD();
}

void startTimeoutTimer() {
	CommState.receivingTimeoutTimer = addTimer(200, FALSE,
			&COMM_PeriodElapsedCallback);
}

void stopTimeoutTimer() {
	if (CommState.receivingTimeoutTimer > 0) {
		removeTimer(CommState.receivingTimeoutTimer);
		CommState.receivingTimeoutTimer = 0;
	}
}

void restartTimeoutTimer() {
	stopTimeoutTimer();
	startTimeoutTimer();
}

void COMM_KeepAlivePeriodElapsedCallback() {
	CommState.CommDriverReady = FALSE;
	COMM_ResumeTaskFromISR();
}

void COMM_HalfKeepAlivePeriodElapsedCallback() {
	CommState.HalfKeepAliveFlag = TRUE;
	checkOverRun();
	COMM_ResumeTaskFromISR();
}

void resetKeepAliveWatchDog() {
	lastTimeStampReset = timeStampCounter;
	if (CommState.keepAliveTimer > 0) {
		restartTimer(CommState.keepAliveTimer);
	} else {
		CommState.keepAliveTimer = addTimer(KEEP_ALIVE_PERIOD, FALSE,
				&COMM_KeepAlivePeriodElapsedCallback);
	}
	if (CommState.halfKeepAliveTimer>0){
		restartTimer(CommState.halfKeepAliveTimer);
	}else{
		CommState.halfKeepAliveTimer = addTimer(KEEP_ALIVE_PERIOD/2, FALSE,
				&COMM_HalfKeepAlivePeriodElapsedCallback);
	}
}

void stopKeppAliveTimer() {
	if (CommState.keepAliveTimer > 0) {
		removeTimer(CommState.keepAliveTimer);
		CommState.keepAliveTimer = 0;
	}
	if (CommState.halfKeepAliveTimer>0){
		removeTimer(CommState.halfKeepAliveTimer);
		CommState.halfKeepAliveTimer = 0;
	}
}

void commSleep() {
	const TickType_t xBlockTime = pdMS_TO_TICKS(10);
	ulTaskNotifyTake(pdFALSE, xBlockTime);
}

void notifyPacketProcessed() {
	CommState.RxState = RxProcessed;
	xTaskNotifyGive(commHandle);
	taskYIELD();
}

void create_rx_err(u8 err) {

	volatile u8* bodyPtr = &debugPacket.body[0];
	*bodyPtr++ = err;
	*bodyPtr++ = (u8) CommState.rxIndex;
	*bodyPtr++ = (u8) stateBeforeTimeout;

	for (int i = 0; i < DEBUG_BODY_SIZE - 3; i++) {
		*bodyPtr++=commInBuf[i];
	}
	debugPacket.command = 0x01;
	debugPacket.size = 10;
	debugPacket.pending = TRUE;

	CommState.rxIndex = 0;
	CommState.RxState = WaitingStart;
	stopTimeoutTimer();
}

void COMM_Task() {

	int i = 0;

	while (1) {

		//no keep alive packet
		if (CommState.CommDriverReady == FALSE) {
			stopTimeoutTimer();
			stopKeppAliveTimer();
			CommState.rxIndex = 0;
			CommState.RxState = WaitingStart;
			CommState.TxState = TxIdle;
			CommState.AtLeastOnePacketReceived = FALSE;
			COMM_Driver_Configure();
			CommState.rxIndex = 0;
			CommState.RxState = WaitingStart;
			CommState.TxState = TxIdle;
			continue;
		}

		i = 0;
		if (CommState.RxState == WaitingStart) {
			//ждем новый пакет
			//статус может изменить таймер таймаута
			if (CommState.rxIndex == 0) {
				//получили первый байт, ждем еще 7-8
				stopTimeoutTimer();
				commSleep();
				continue;
			} else if (CommState.rxIndex > 0) {
				if (commInBuf[0] != PACKET_START) {
					create_rx_err(0x01);
					continue;
				} else {
					CommState.RxState = ReceivingSize;
					CommState.rxPacketSize = 0;
					restartTimeoutTimer();
				}
			}
		} else if (CommState.RxState == ReceivingSize) {
			if (CommState.rxIndex > 3) {
				u16 inPacksize = commInBuf[1] + (commInBuf[2] * 256);
				if (inPacksize < EMPTY_PACKET_SIZE) {
					create_rx_err(0x02);
					continue;
				}
				if (inPacksize > COMM_IN_BUF_SIZE) {
					create_rx_err(0x03);
					continue;
				}
				CommState.rxPacketSize = inPacksize;
				CommState.RxState = ReceivingPacket;
			} else {
				commSleep();
			}
		} else if (CommState.RxState == ReceivingPacket) {
			if (CommState.rxIndex >= CommState.rxPacketSize) {
				//Пакет пришел. останавливаем таймер и проверяем его
				stopTimeoutTimer();
				CommState.RxState = RxChecking;
			} else {
				commSleep();
			}
		} else if (CommState.RxState == ReceivingTimeout) {
			stopTimeoutTimer();
			resetKeepAliveWatchDog();
			create_rx_err(0x06);
		} else if (CommState.RxState == RxChecking) {
			//останавливаем таймер таймаута так как пришел весь пакет
			stopTimeoutTimer();
			//check CRC
			u8 crc = 0;
			//265-2=263
			for (i = 0; i < CommState.rxPacketSize - 2; i++) {
				crc += commInBuf[i];
			}
			u8 xorCRC = crc ^ 0xAA;
			//если контрольная сумма сошлась
			if (crc == commInBuf[CommState.rxPacketSize - 2]
					&& xorCRC == commInBuf[CommState.rxPacketSize - 1]) {
				CommState.RxState = RxProcessing;
				stopTimeoutTimer();
				processPacket();

			} else {			//crc error
				create_rx_err(0x04);
			}
		} else if (CommState.RxState == RxProcessing) {
			commSleep();
		}
		if (CommState.RxState == RxProcessed) {
			CommState.rxIndex = 0;
			CommState.RxState = WaitingStart;
		}
	}
}

void createOutPacketAndSend(u8 command, u16 bodySize, u8* bodyData) {
	checkOverRun();
	if (CommState.TxState == TxSending) {
		return;
	}

	u16 i = 0;
	u16 txBufSize = 6 + bodySize;
	commOutBuf[0] = PACKET_START;
	commOutBuf[1] = (u8) (txBufSize);
	commOutBuf[2] = (u8) (txBufSize >> 8);
	commOutBuf[3] = command;

	if (bodySize > 0 && bodyData != NULL) {
		for (i = 0; i < bodySize; i++) {
			commOutBuf[COMM_OUT_BODY_OFFSET + i] = *(bodyData + i);
		}
	}

	u8 crc = 0;
	for (i = 0; i < 4 + bodySize; i++) {
		crc += commOutBuf[i];
	}
	commOutBuf[4 + bodySize] = crc;
	commOutBuf[5 + bodySize] = crc ^ 0xAA;

	COMM_SendData(txBufSize);
}

void processPacket() {
	CommState.AtLeastOnePacketReceived = TRUE;
	resetKeepAliveWatchDog();

/*
	volatile u8* bodyPtr = &debugPacket.body[0];
	*bodyPtr++ = 35;
	*bodyPtr++ = 35;
	*bodyPtr++ = 35;

	for (int i = 0; i < DEBUG_BODY_SIZE - 3; i++) {
		*bodyPtr++=commInBuf[i];
	}
	debugPacket.command = 0x01;
	debugPacket.size = 3;
	debugPacket.pending = TRUE;
*/

	if (commInBuf[3] == 0x01) {
		CommState.RxState = RxProcessed;
	}

	//for debug reasons
	CommState.RxState = RxProcessed;
}

