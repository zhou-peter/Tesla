/*
 * packet_manager.c
 *
 *  Created on: 20 но€б. 2018 г.
 *      Author: User
 */
#include "common.h"
#include "cmsis_os.h"
#include "timers.h"
#include "packet_manager.h"
#include "kernel.h"

extern void PM_TxComplete(DMA_HandleTypeDef * hdma);
extern void vTimerStateSend(TimerHandle_t xTimer);
extern void vTimerCallback( TimerHandle_t xTimer );
extern void processPacket();
#define STATE_SEND_PERIOD 200
//если за 0.5 секунды не получен пакет, что-то не так
#define RECEIVE_TIMEOUT 500
#define BODY_OFFSET 4
#define EMPTY_SIZE 6

void resetRxBuf(){
	int i=0;
	Uart.rxIndex=0;
	for(i=0;i<RX_BUF_SIZE;i++){
		Uart.rxBuf[i]=0;
	}
}

TimerHandle_t xTimerState;
TimerHandle_t xTimerTimeout;



void PM_Init(){
	Uart.RxState=WaitingStart;

	xTimerTimeout = xTimerCreate
              ( /* Just a text name, not used by the RTOS
                kernel. */
                "Rx Timeout",
                /* The timer period in ticks, must be
                greater than 0. */
				pdMS_TO_TICKS(RECEIVE_TIMEOUT),
                /* The timers will auto-reload themselves
                when they expire. */
				pdFALSE,
                /* The ID is used to store a count of the
                number of times the timer has expired, which
                is initialised to 0. */
                ( void * ) 0,
                /* Each timer calls the same callback when
                it expires. */
                vTimerCallback
              );
	xTimerState=xTimerCreate("Tx State", pdMS_TO_TICKS(STATE_SEND_PERIOD), pdTRUE, ( void * ) 0,
			vTimerStateSend);
	hdma_usart1_tx.XferCpltCallback=PM_TxComplete;

	HAL_UART_Receive_IT(&huart1, &Uart, 1);
}

void startTimer(){
	xTimerReset(xTimerTimeout,100);
    /* Start the timer.  No block time is specified, and
    even if one was it would be ignored because the RTOS
    scheduler has not yet been started. */
    if( xTimerStart( xTimerTimeout, 0  ) != pdPASS )
    {
        /* The timer could not be set into the Active
        state. */
    	configASSERT( xTimerTimeout );
    }
    }
void stopTimer(){
	if (xTimerStop( xTimerTimeout,100)==pdFAIL){
		configASSERT(xTimerTimeout);
	}
}

void restartTimer(){
	stopTimer();
	startTimer();
}
void create_rx_err(u8 err){
	Uart.rxIndex=0;
	Uart.RxState=WaitingStart;
	stopTimer();
	if (Uart.TxState==TxIdle){
		createOutPacketAndSend(0x03,1,&err);
	}
}

void onRx(){
	if (Uart.rxIndex<RX_BUF_SIZE-1){
		Uart.rxBuf[Uart.rxIndex]=Uart.halRxBuf;
		Uart.rxIndex++;
	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	onRx();
	HAL_UART_Receive_IT(&huart1, &Uart, 1);
}
void vTimerCallback( TimerHandle_t xTimer )
{
	Uart.RxState=ReceivingTimeout;
}
void PM_Task(){
    if( xTimerStart( xTimerState, pdMS_TO_TICKS(1000) ) != pdPASS )
    {
    	configASSERT( xTimerTimeout );
    }

	int i=0;

	while(1){

		i=0;
		if (Uart.RxState==WaitingStart){
			//ждем новый пакет
			//статус может изменить таймер таймаута
			if (Uart.rxIndex==0){
				//получили первый байт, ждем еще 7-8
				osDelay(4);
				continue;
			}
			else if (Uart.rxIndex>0)
			{
				if (Uart.rxBuf[0]!=PACKET_START){
					create_rx_err(0x01);
					continue;
				}else{
					Uart.RxState=ReceivingSize;
					Uart.rxPackSize=0;
					restartTimer();
				}
			}
		}else if (Uart.RxState==ReceivingSize){
			if (Uart.rxIndex>3){
				u16 inPacksize=Uart.rxBuf[1]+
						(Uart.rxBuf[2]*256);
				if (inPacksize<6){
					create_rx_err(0x02);
					continue;
				}
				if (inPacksize>RX_BUF_SIZE-EMPTY_SIZE){
					create_rx_err(0x03);
					continue;
				}
				Uart.rxPackSize=inPacksize;
				Uart.RxState=ReceivingPacket;
			}else{
				osDelay(3);
			}
		}else if (Uart.RxState==ReceivingPacket){
			if (Uart.rxIndex>=Uart.rxPackSize){
				//ѕакет пришел. останавливаем таймер и провер€ем его
				stopTimer();
				Uart.RxState=RxChecking;
			}else{
				osDelay(10);
			}
		}else if (Uart.RxState==ReceivingTimeout){
			stopTimer();
			create_rx_err(0x06);
		}else if(Uart.RxState==RxChecking){
			//останавливаем таймер таймаута так как пришел весь пакет
			stopTimer();
			//check CRC
			u8 crc=0;
			//265-2=263
			for (i=0;i<Uart.rxPackSize-2;i++){
				crc+=Uart.rxBuf[i];
			}
			u8 xorCRC=crc^0xAA;
			//если контрольна€ сумма сошлась
			if (crc==Uart.rxBuf[Uart.rxPackSize-2]&&
					xorCRC==Uart.rxBuf[Uart.rxPackSize-1]){
				Uart.RxState=RxProcessing;
				stopTimer();
				processPacket();

			}else{//crc error
				create_rx_err(0x04);
			}
		}else if(Uart.RxState==RxProcessing){
			osDelay(2);
		}else if(Uart.RxState==RxProcessed){
			Uart.rxIndex=0;
			Uart.RxState=WaitingStart;
		}

	}


}



void processPacket(){
	Uart.RxState=RxProcessing;
	u8 packetCode=Uart.rxBuf[3];
	u8* body=&Uart.rxBuf[BODY_OFFSET];
	u16 bodySize=Uart.rxPackSize-EMPTY_SIZE;
	switch(packetCode){
		case 0x02:
			packet_02_feature_change(body, bodySize);
			break;
		case 0x04:
			packet_04_timer_config(body, bodySize);
			break;
	}
	Uart.RxState=RxProcessed;
}

void createOutPacketAndSend(u8 command, u16 bodySize, u8* bodyData){

	if (Uart.TxState==Sending)
	{
		return;
	}



		u16 i=0;
		Uart.txBufSize=6+bodySize;
		Uart.txBuf[0]=PACKET_START;
		Uart.txBuf[1]=(u8)(Uart.txBufSize);
		Uart.txBuf[2]=(u8)(Uart.txBufSize>>8);
		Uart.txBuf[3]=command;

		if (bodySize>0){
			for (i=0;i<bodySize;i++){
				Uart.txBuf[4+i]=*(bodyData+i);
			}
		}

		u8 crc=0;
		for (i=0;i<4+bodySize;i++){
			crc+=Uart.txBuf[i];
		}
		Uart.txBuf[4+bodySize]=crc;
		Uart.txBuf[5+bodySize]=crc^0xAA;


		Uart.txIndex=0;

		HAL_UART_Transmit_DMA(&huart1, &Uart.txBuf, Uart.txBufSize);
		//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}
u8 sentTimes=0;
void vTimerStateSend(TimerHandle_t xTimer )
{
	createOutPacketAndSend(0x01, 8, &State);
	sentTimes++;
	if (sentTimes>5){
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12, GPIO_PIN_RESET);
		State.LedLight=FALSE;
		//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_12);
	}else{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12, GPIO_PIN_SET);
		State.LedLight=TRUE;
	}
	if (sentTimes>10){
		sentTimes=0;
	}
}
void PM_TxComplete(DMA_HandleTypeDef * hdma){
	Uart.TxState=TxIdle;
}
