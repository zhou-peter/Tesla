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
	//ATNameGet,
	//ATNameGetAnswerWait,
	ATNameSet,
	ATNameSetAnswerWait,
	ATPinGet,
	ATPinGetAnswerWait,
	ATPin,
	ATPinAnswerWait,
	ATSpeed,
	ATSpeedAT2, //again send AT command
	ATSpeedAT2AnswerWait,
	ATWaitStream,
	ATInitFail
}ATStates;


typedef struct
{
	//indicate that answer from previous command
	//came as expected
	ATStates ATState:8;
}HCModule_t;


extern void COMM_Configure_Driver(UART_HandleTypeDef* uart_,
		DMA_HandleTypeDef* hdma_usart_, TIM_HandleTypeDef* timer,
		osThreadId_t taskHandle);
extern void COMM_RxCallback();


#endif /* COMMUNICATION_HC0X_H_ */
