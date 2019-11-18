/*
 * communication_hc0x.h
 *
 *  Created on: 30 окт. 2019 г.
 *      Author: User
 */
#include "common.h"

#ifndef COMMUNICATION_HC0X_H_
#define COMMUNICATION_HC0X_H_


typedef enum
{
	AT = 0,
	ATAnswerWait,
	ATNameGet,
	ATNameGetAnswerWait,
	ATNameSet,
	ATNameSetAnswerWait,
	ATPinGet,
	ATPinGetAnswerWait,
	ATPin,
	ATPinAnswerWait,
	ATSpeed,
	ATSpeedAnswerWait,
	ATWaitStream
}ATStates;


typedef struct
{
	bool	ATTimeout:1;
	ATStates ATState:7;
	u8		softTimer:8;
	u16		tmp4:16;
}HCModule_t;


extern void COMM_Configure_Driver(UART_HandleTypeDef* uart_,
		DMA_HandleTypeDef* hdma_usart_,
		TaskHandle_t taskHandle);
extern void COMM_RxCallback();


#endif /* COMMUNICATION_HC0X_H_ */
