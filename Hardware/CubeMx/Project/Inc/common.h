#ifndef __COMMON_H
#define __COMMON_H


#include <stdio.h>

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

typedef enum {FALSE = 0, TRUE = !FALSE} bool;
#define u32 uint32_t
#define s32 int32_t
#define u16 uint16_t
#define s16 int16_t
#define u8 uint8_t
#define s8 int8_t

#define RX_BUF_SIZE 80
#define TX_BUF_SIZE 80

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;

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
	Sending
}TxStates;
typedef enum
{
	PokerIdle=0,
	PokerRotatingTime,
	PokerRotatingLowPoint
} PokerStates;

typedef struct
{
bool TimerF1:8;
bool TimerF2:8;
bool TimerF3:8;
bool TimerF4:8;
bool LedLight:8;
bool Tmp1:8;
bool Tmp2:8;
bool Tmp3:8;
} State_t;

//структура должна быть выравнена по 32 бита по модулю
typedef struct
{
u32 tmp0:24;

RxStates RxState:4;
TxStates TxState:4;//16
u16 rxIndex;
u16 txIndex;
u16 txBufSize;
u16 rxPackSize;


u8 rxBuf[RX_BUF_SIZE];
u8 txBuf[TX_BUF_SIZE];
} UART_t;




extern volatile UART_t Uart;
extern volatile State_t State;

#endif
