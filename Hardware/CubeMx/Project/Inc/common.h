#ifndef __COMMON_H
#define __COMMON_H


#include <stdio.h>



typedef enum {FALSE = 0, TRUE = !FALSE} bool;
#define u32 uint32_t
#define s32 int32_t
#define u16 uint16_t
#define s16 int16_t
#define u8 uint8_t
#define s8 int8_t

#define RX_BUF_SIZE 80
#define TX_BUF_SIZE 80



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

//структура должна быть выравнена по 32 бита по модулю
typedef struct
{
bool TimerF1:8;
bool TimerF1:8;
bool TimerF1:8;
bool TimerF1:8;
bool tmp1:1;
bool Pausing:1;


RxStates RxState:4;
TxStates TxState:4;//16
u16 rxIndex;
u16 txIndex;
u16 txBufSize;
u16 rxPackSize;


u8 rxBuf[RX_BUF_SIZE];
u8 txBuf[TX_BUF_SIZE];
} Env_t;




extern volatile Env_t Env;


#endif
