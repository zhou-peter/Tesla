/*
 * communication.c
 *
 *  Created on: 30 окт. 2019 г.
 *      Author: User
 */
#include "communication_manager.h"

volatile u8 commInBuf[COMM_IN_BUF_SIZE];
volatile u8 commOutBuf[COMM_OUT_BUF_SIZE];
volatile CommState_t CommState;

TaskHandle_t commHandle;
TIM_HandleTypeDef* tim;



void COMM_Init(TIM_HandleTypeDef* timer, TaskHandle_t taskHandle)
{
	tim = timer;
	commHandle = taskHandle;

}



COMM_Task()
{

}

void COMM_PeriodElapsedCallback()
{

}
