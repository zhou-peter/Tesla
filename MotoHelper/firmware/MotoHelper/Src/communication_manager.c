/*
 * communication.c
 *
 *  Created on: 30 окт. 2019 г.
 *      Author: User
 */
#include "communication_manager.h"
#include "soft_timer.h"

#define PACKET_START 0xCC
//100ms Timer
#define RECEIVE_TIMEOUT 1000/100





volatile u8 commInBuf[COMM_IN_BUF_SIZE];
volatile u8 commOutBuf[COMM_OUT_BUF_SIZE];

volatile CommState_t CommState;
volatile u16 rxIndex;
TaskHandle_t commHandle;


void COMM_Init(TaskHandle_t taskHandle)
{
	commHandle = taskHandle;
}

void COMM_PeriodElapsedCallback()
{
	switch (CommState.RxState){
		case ReceivingSize:
		case ReceivingPacket:
			CommState.RxState=ReceivingTimeout;
			break;
		default:
			break;
	}
	BaseType_t false = pdFALSE;
	vTaskNotifyGiveFromISR(commHandle, &false);
	taskYIELD();
}


void startTimer(){
	CommState.softTimer = addTimer(500, FALSE,
			&COMM_PeriodElapsedCallback);
}


void stopTimer(){
	if(CommState.softTimer>0){
		removeTimer(CommState.softTimer);
		CommState.softTimer = 0;
	}
}

void restartTimer(){
	stopTimer();
	startTimer();
}

void commSleep(){
	const TickType_t xBlockTime = pdMS_TO_TICKS(5000);
	ulTaskNotifyTake(pdFALSE, xBlockTime);
}


void notifyPacketProcessed()
{
	CommState.RxState=RxProcessed;
	xTaskNotifyGive(commHandle);
	taskYIELD();
}



void create_rx_err(u8 err){
	CommState.rxIndex=0;
	CommState.RxState=WaitingStart;
	stopTimer();
	if (CommState.TxState==TxIdle)
	{
		//createOutPacketAndSend(0x03,1,&err);
	}
}

void resetRxBuf(){
	int i=0;
	CommState.rxIndex=0;
	for(i=0;i<COMM_IN_BUF_SIZE;i++){
		commInBuf[i]=0;
	}
}

void COMM_Task(){


	int i=0;

	while(1){

		i=0;
		if (CommState.RxState==WaitingStart){
			//ждем новый пакет
			//статус может изменить таймер таймаута
			if (CommState.rxIndex==0){
				//получили первый байт, ждем еще 7-8
				stopTimer();
				commSleep();
				continue;
			}
			else if (CommState.rxIndex>0)
			{
				if (commInBuf[0]!=PACKET_START){
					create_rx_err(0x01);
					continue;
				}else{
					CommState.RxState=ReceivingSize;
					CommState.rxPacketSize=0;
					restartTimer();
				}
			}
		}else if (CommState.RxState==ReceivingSize){
			if (CommState.rxIndex>3){
				u16 inPacksize=commInBuf[1]+
						(commInBuf[2]*256);
				if (inPacksize<6){
					create_rx_err(0x02);
					continue;
				}
				if (inPacksize>COMM_OUT_MAX_BODY_SIZE){
					create_rx_err(0x03);
					continue;
				}
				CommState.rxPacketSize=inPacksize;
				CommState.RxState=ReceivingPacket;
			}else{
				commSleep();
			}
		}else if (CommState.RxState==ReceivingPacket){
			if (CommState.rxIndex>=CommState.rxPacketSize){
				//Пакет пришел. останавливаем таймер и проверяем его
				stopTimer();
				CommState.RxState=RxChecking;
			}else{
				commSleep();
			}
		}else if (CommState.RxState==ReceivingTimeout){
			stopTimer();
			create_rx_err(0x06);
		}else if(CommState.RxState==RxChecking){
			//останавливаем таймер таймаута так как пришел весь пакет
			stopTimer();
			//check CRC
			u8 crc=0;
			//265-2=263
			for (i=0;i<CommState.rxPacketSize-2;i++){
				crc+=commInBuf[i];
			}
			u8 xorCRC=crc^0xAA;
			//если контрольная сумма сошлась
			if (crc==commInBuf[CommState.rxPacketSize-2]&&
					xorCRC==commInBuf[CommState.rxPacketSize-1]){
				CommState.RxState=RxProcessing;
				stopTimer();
				processPacket();

			}else{//crc error
				create_rx_err(0x04);
			}
		}else if(CommState.RxState==RxProcessing){
			commSleep();
		}else if(CommState.RxState==RxProcessed){
			CommState.rxIndex=0;
			CommState.RxState=WaitingStart;
		}
	}
}



void processPacket(){
	CommState.RxState=RxProcessing;
	commSleep();

}


void createOutPacketAndSend(u8 command, u16 bodySize, u8* bodyData){

	if (CommState.TxState==TxSending)
	{
		return;
	}

	u16 i=0;
	u16 txBufSize=6+bodySize;
	commOutBuf[0]=PACKET_START;
	commOutBuf[1]=(u8)(txBufSize);
	commOutBuf[2]=(u8)(txBufSize>>8);
	commOutBuf[3]=command;

	if (bodySize>0 && bodyData !=NULL){
		for (i=0;i<bodySize;i++){
			commOutBuf[COMM_OUT_BODY_OFFSET+i]=*(bodyData+i);
		}
	}

	u8 crc=0;
	for (i=0;i<4+bodySize;i++){
		crc+=commOutBuf[i];
	}
	commOutBuf[4+bodySize]=crc;
	commOutBuf[5+bodySize]=crc^0xAA;


	COMM_SendData(txBufSize);
}


