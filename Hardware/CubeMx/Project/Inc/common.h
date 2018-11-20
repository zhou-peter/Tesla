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
	ATMode = 0,
	StreamMode
}BT_Module_States;
typedef enum
{
	AT = 0,
	ATAnswerWait,
	ATNameGet,
	ATNameGetAnswerWait,
	ATName,
	ATNameAnswerWait,
	ATPinGet,
	ATPinGetAnswerWait,
	ATPin,
	ATPinAnswerWait,
	ATWaitStream
}ATStates;
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
typedef enum
{
	FuelIdle=0,
	FuelRotatingTime
} FuelStates;
typedef enum
{
	IdleNoConf=0,
	IdleLowPower,
	Working,
	WorkingVoiteDontWork
} MainLogicStates;
typedef enum
{
	ChargeIdle=0,
	ChargeInvalidConfig,
	ChargeCharging,
	ChargeBackuped
} ChargeStates;
typedef enum
{
	ButtonUnpressed=0,
	ButtonPressing,		//пикаем пол секунды
	ButtonPressed,
	ButtonUnpressing	//пикаем пол секунды -> ButtonUnpressed
}ButtonStates;
typedef enum
{
	SoundDisabled=0,
	SoundEnabled
}SoundStates;
typedef enum
{
	LedDisabled=0,
	LedEnabled
}LedStates;
typedef enum
{
	SPI_Idle=0,
	SPI_Communicating,
	SPI_Processing,
	SPI_Processed
}SpiStates;

//структура должна быть выравнена по 32 бита по модулю
typedef struct
{
bool OneSecondTmr:1;
bool HundredMillisecondTmr:1;
bool TenMillisecondTmr:1;
bool PulseEnabled:1;
bool CellRunning:1;
bool Pausing:1;

u16 TimerCounter:10;//16

MainLogicStates MLState:8;
ButtonStates ButtonState:4;
u8 CellPacketsCounter:4;//16

LedStates LedState:4;
SoundStates SoundState:4;
bool SearchRunning:1;
u8 tmp2:7;//16

u16 pwmPulseDelay:16;

u32 pwmFreq:32;//index8
u32 pwmPulseFreq:32;
u16 pwmPayload:16;
u16 pwmPulsePayload:16;
u8 pwmPausePeriod:8;
u8 pwmPauseDutyCycle:8;//16


u16 voltage:16;
u16 current:16;


u32 uptime;
	BT_Module_States BT_Module_State:2;
	bool ATTimer:1;
	bool ATAnswerOK:1;
	ATStates ATState:4;
	RxStates RxState:4;
	TxStates TxState:4;//16

	u16 DAC1Value:12;

  u16 rxIndex;
  u16 txIndex;
  u16 txBufSize;
  u16 rxPackSize;


  u8 rxBuf[RX_BUF_SIZE];
  u8 txBuf[TX_BUF_SIZE];
} Env_t;

//40 bytes
typedef struct
{
	bool HasCalibration:1;
	u8 tmp0:7;
	u8	Ver:8;
	u16 FlashCRC:16;
	u32 Tmp2:32;
} ConfData_t;



extern volatile Env_t Env;


#endif
