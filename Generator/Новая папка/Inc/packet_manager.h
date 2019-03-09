/*
 * packet_manager.h
 *
 *  Created on: 20 но€б. 2018 г.
 *      Author: User
 */
#include "common.h"

#ifndef PACKET_MANAGER_H_
#define PACKET_MANAGER_H_
#define PACKET_START 0xCC

extern void PM_Init();
extern void PM_Task();
extern void createOutPacketAndSend(u8 command, u16 bodySize, u8* bodyData);
extern void onRx();

#endif /* PACKET_MANAGER_H_ */
