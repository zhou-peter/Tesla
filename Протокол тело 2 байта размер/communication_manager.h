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
//при скорости 115200 пакет из 80 байт будет предаваться
//по юарту 7мс
#define COMM_OUT_BUF_SIZE 80
//размер тела специального дебаг пакета
#define DEBUG_BODY_SIZE 10

#define COMM_OUT_BODY_OFFSET 4
#define EMPTY_PACKET_SIZE 6
#define COMM_OUT_MAX_BODY_SIZE (COMM_OUT_BUF_SIZE-EMPTY_PACKET_SIZE)
#define COMM_IN_MAX_BODY_SIZE (COMM_IN_BUF_SIZE-EMPTY_PACKET_SIZE)

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

typedef struct
{
	bool pending:1;
	u8 size:7;
	u8 command;
	u8 body[DEBUG_BODY_SIZE];
}OutPacket;

//структура должна быть выравнена по 32 бита по модулю
typedef struct
{
	RxStates RxState:4;
	TxStates TxState:2;
	bool	AtLeastOnePacketReceived:1;
	bool	CommDriverReady:1;
	bool	HighSpeed:1;	//9600/115200
	bool	RxResetRequired:1;
	u32		tmp3:6;
	u8 receivingTimeoutTimer:8;//receiving timeout
	u8 keepAliveTimer:8;//no keep-alive timeout
	u16 rxPacketSize:16;
	u16 rxIndex:16;
} CommState_t;



extern TaskHandle_t commHandle;
extern volatile CommState_t CommState;

extern void COMM_Driver_HealthCheck();
extern volatile u8 commInBuf[COMM_IN_BUF_SIZE];
extern volatile u8 commOutBuf[COMM_OUT_BUF_SIZE];
extern volatile OutPacket debugPacket;

extern void COMM_Init(TaskHandle_t taskHandle);
extern void COMM_Task();
extern void COMM_ResumeTaskFromISR();

extern void createOutPacketAndSend(u8 command, u16 bodySize, u8* bodyData);
extern void notifyPacketProcessed();
#endif /* COMMUNICATION_H_ */
