/*
 * communication.h
 *
 *  Created on: 30 окт. 2019 г.
 *      Author: User
 */
#include "common.h"

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#define COMM_IN_BUF_SIZE 20
#define COMM_OUT_BUF_SIZE 80

typedef enum
{
	WaitingStart,
	ReceivingSize,
	ReceivingPacket,
	ReceivingTimeout,
	RxChecking,
	RxProcessing,
	RxProcessed
} RxStates;

typedef enum
{
	TxIdle,
	TxSending
}TxStates;

//структура должна быть выравнена по 32 бита по модулю
typedef struct
{
	RxStates RxState:4;
	TxStates TxState:4;//16
	bool RxEvent:8; //new byte came
} CommState_t;












extern TaskHandle_t commHandle;
extern volatile CommState_t CommState;

extern volatile u8 commInBuf[COMM_IN_BUF_SIZE];
extern volatile u8 commOutBuf[COMM_OUT_BUF_SIZE];

extern void COMM_Init(TIM_HandleTypeDef* timer, TaskHandle_t taskHandle);
extern void COMM_PeriodElapsedCallback();
extern void COMM_Task();

#endif /* COMMUNICATION_H_ */
