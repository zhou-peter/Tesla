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

extern void PM_TxComplete(DMA_HandleTypeDef * hdma);
extern void vTimerStateSend(TimerHandle_t xTimer);

#define STATE_SEND_PERIOD 200
//если за 0.5 секунды не получен пакет, что-то не так
#define RECEIVE_TIMEOUT 500

void addRxByte(u8 rxByte){
    	if (Env.rxIndex<RX_BUF_SIZE-1){
    		Env.rxBuf[Env.rxIndex]=rxByte;
    		Env.rxIndex++;
    		return;
    	}
/*
    if (USART_GetFlagStatus(USART1,USART_FLAG_TXE)==SET)
	{
		if(Env.TxState==Sending){
			if (Env.txIndex<Env.txBufSize){
				USART_SendData(USART1, Env.txBuf[Env.txIndex]);
				//while (USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
				Env.txIndex++;
			}else{
				Env.txIndex=0;
				Env.TxState=TxIdle;
				USART_ClearFlag(USART1,USART_FLAG_TXE);
				USART_ClearITPendingBit(USART1, USART_IT_TXE);
				USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
			}
		}
	}
    */
}
void PacketTimeOut(void)
{
	Env.RxState=ReceivingTimeout;
}



void resetRxBuf(){
	int i=0;
	Env.rxIndex=0;
	for(i=0;i<RX_BUF_SIZE;i++){
		Env.rxBuf[i]=0;
	}
}

TimerHandle_t xTimerState;
TimerHandle_t xTimerTimeout;
void vTimerCallback( TimerHandle_t xTimer )
{
	Env.RxState=ReceivingTimeout;
}


void PM_Init(){
	Env.RxState=WaitingStart;
	xTimerTimeout = xTimerCreate
              ( /* Just a text name, not used by the RTOS
                kernel. */
                "Rx Timeout",
                /* The timer period in ticks, must be
                greater than 0. */
				pdMS_TO_TICKS(RECEIVE_TIMEOUT),
                /* The timers will auto-reload themselves
                when they expire. */
				pdTRUE,
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
}

void startTimer(){
    /* Start the timer.  No block time is specified, and
    even if one was it would be ignored because the RTOS
    scheduler has not yet been started. */
    if( xTimerStart( xTimerTimeout, 0 ) != pdPASS )
    {
        /* The timer could not be set into the Active
        state. */
    	configASSERT( xTimerTimeout );
    }
}
void stopTimer(){
	xTimerStop( xTimerTimeout,0);
}
void resetTImer(){
	xTimerReset(xTimerTimeout,0);
}
void restartTimer(){
	stopTimer();
	resetTImer();
	startTimer();
}
void create_rx_err(u8 err){
	Env.rxIndex=0;
	Env.RxState=WaitingStart;
	stopTimer();
	resetTImer();
	if (Env.TxState==TxIdle){
		createOutPacketAndSend(0x02,1,&err);
	}
}
void PM_Task(){
    if( xTimerStart( xTimerState, pdMS_TO_TICKS(1000) ) != pdPASS )
    {
    	configASSERT( xTimerTimeout );
    }

	int i=0;

	while(1){

			i=0;
		if (Env.RxState==WaitingStart){
			//ждем новый пакет
			//статус может изменить таймер таймаута
			if (Env.rxIndex==0){
				//получили первый байт, ждем еще 7-8
				osDelay(10);
				continue;
			}
			else if (Env.rxIndex>0 && Env.rxBuf[0]!=PACKET_START){
				Env.rxIndex=0;
				create_rx_err(0x01);
				continue;
			}else{
				Env.RxState=ReceivingSize;
				Env.rxPackSize=0;
				restartTimer();
			}
		}else if (Env.RxState==ReceivingSize){
			if (Env.rxIndex>3){
				u16 inPacksize=Env.rxBuf[1]+
						(Env.rxBuf[2]*256);
				if (inPacksize<6){
					create_rx_err(0x02);
					continue;
				}
				if (inPacksize>RX_BUF_SIZE-6){
					create_rx_err(0x03);
					continue;
				}
				Env.rxPackSize=inPacksize;
				Env.RxState=ReceivingPacket;
			}else{
				osDelay(3);
			}
		}else if (Env.RxState==ReceivingPacket){
			if (Env.rxIndex>=Env.rxPackSize){
				//ѕакет пришел. останавливаем таймер и провер€ем его
				stopTimer();
				Env.RxState=RxChecking;
			}else{
				osDelay(10);
			}
		}else if (Env.RxState==ReceivingTimeout){
			create_rx_err(0x06);
		}else if(Env.RxState==RxChecking){
			//останавливаем таймер таймаута так как пришел весь пакет
			stopTimer();
			//check CRC
			u8 crc=0;
			//265-2=263
			for (i=0;i<Env.rxPackSize-2;i++){
				crc+=Env.rxBuf[i];
			}
			u8 xorCRC=crc^0xAA;
			//если контрольна€ сумма сошлась
			if (crc==Env.rxBuf[Env.rxPackSize-2]&&
					xorCRC==Env.rxBuf[Env.rxPackSize-1]){
				Env.RxState=RxProcessing;

			}else{//crc error
				create_rx_err(0x04);
			}
		}else if(Env.RxState==RxProcessing){
			osDelay(2);
		}else if(Env.RxState==RxProcessed){
			Env.rxIndex=0;
			Env.RxState=WaitingStart;
		}

		}


}


void createOutPacketAndSend(u8 command, u8 bodySize, u8* bodyData){

	if (Env.TxState==Sending)
	{
		return;
	}



		u16 i=0;
		Env.txBufSize=6+bodySize;
		Env.txBuf[0]=PACKET_START;
		Env.txBuf[1]=(u8)(Env.txBufSize);
		Env.txBuf[2]=(u8)(Env.txBufSize>>8);
		Env.txBuf[3]=command;

		if (bodySize>0){
			for (i=0;i<bodySize;i++){
				Env.txBuf[4+i]=*(bodyData+i);
			}
		}

		u8 crc=0;
		for (i=0;i<4+bodySize;i++){
			crc+=Env.txBuf[i];
		}
		Env.txBuf[4+bodySize]=crc;
		Env.txBuf[5+bodySize]=crc^0xAA;


		Env.txIndex=0;

		HAL_UART_Transmit_DMA(&huart1, &Env.txBuf, Env.txBufSize);
		//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}
u8 sentTimes=0;
void vTimerStateSend(TimerHandle_t xTimer )
{
	createOutPacketAndSend(0x01, 8, &Env);
	sentTimes++;
	if (sentTimes>2){
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12, GPIO_PIN_RESET);
		//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_12);
	}else{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12, GPIO_PIN_SET);
	}
	if (sentTimes>5){
		sentTimes=0;
	}
}
void PM_TxComplete(DMA_HandleTypeDef * hdma){
	Env.TxState=TxIdle;
}
