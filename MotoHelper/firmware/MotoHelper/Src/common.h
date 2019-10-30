#ifndef __COMMON_H
#define __COMMON_H
#define USE_FULL_ASSERT

#include <stdio.h>

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"

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
	SearchIdle=0,
	Searching,
	Generating
} SearcherStates;
typedef enum
{
	SearchPeriodIncrease=0,
	SearchPeriodDecrease
} SearchDirections;


typedef struct
{
bool F1:8;
bool F2:8;
bool F3:8;
bool F4:8;
bool F5:8;
bool F6:8;
bool F10:8;
bool LedLight:8;
SearcherStates SearcherState:4;
bool SearchTimerEnabled:1;
SearchDirections SearchDirection:3;
u8 tmp1:8;
u16	PeriodF1:16;
u16	PeriodF10:16;
} State_t;

//структура должна быть выравнена по 32 бита по модулю
typedef struct
{
u8 halRxBuf:8;
u32 tmp0:24;
RxStates RxState:4;
TxStates TxState:4;//16
u32 tmp1:24;
u16 rxIndex;
u16 txIndex;
u16 txBufSize;
u16 rxPackSize;


u8 rxBuf[RX_BUF_SIZE];
u8 txBuf[TX_BUF_SIZE];
} UART_t;

typedef struct
{
	u16 SearchFrom:16;
	u16 SearchTo:16;
}SearchEnv_t;


typedef struct
{
	u16 Prescaler:16;
	u16 Period:16;
}TimerConf_t;



extern volatile UART_t Uart;
extern volatile State_t State;

extern TimerConf_t calculatePeriodAndPrescaler(u32 freq);

extern s16 getS16(volatile u8* buf, u8 offset);
#endif
