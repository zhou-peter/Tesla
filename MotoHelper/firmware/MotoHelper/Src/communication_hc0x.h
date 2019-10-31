/*
 * communication_hc0x.h
 *
 *  Created on: 30 окт. 2019 г.
 *      Author: User
 */
#include "common.h"

#ifndef COMMUNICATION_HC0X_H_
#define COMMUNICATION_HC0X_H_

extern void COMM_Configure_Driver(UART_HandleTypeDef* uart_, DMA_HandleTypeDef* hdma_usart_);
extern void COMM_RxCallback();


#endif /* COMMUNICATION_HC0X_H_ */
