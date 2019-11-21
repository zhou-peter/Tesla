/*
 * accelerometer_manager.h
 *
 *  Created on: 26 окт. 2019 г.
 *      Author: User
 */
#include "common.h"

#ifndef ACCELEROMETER_MANAGER_H_
#define ACCELEROMETER_MANAGER_H_

//Hz
#define ACCELEROMETER_FREQ	100

typedef enum
{
	AccelIdle,
	AccelConfig,
	AccelReady,		//sleeping
	AccelShouldRequest,
	AccelDataRetriving,//reading data
	AccelDataRetrived,//moved from I2C to tmp buf
	AccelError
} AccelStates_e;


typedef struct
{
	AccelStates_e State:4;
	u32	tmp4:12;
	u32		ms:32;
} AccelState_t;

typedef struct
{
	u16		tmp:16;
	s16		z:16;
	s16		y:16;
	s16		x:16;
} AccelData_t;

extern TaskHandle_t gHandle;
extern volatile AccelState_t 	AccelState;
extern AccelData_t 	AccelData;
extern QueueHandle_t	accelQueue;

extern void ACCEL_Init(I2C_HandleTypeDef *hi2c, TaskHandle_t taskHandle, QueueHandle_t queue);
extern void ACCEL_Task();
extern void ACCEL_NotifyTaskFromISR();
extern void ACCEL_PeriodElapsedCallback();
extern void ACCEL_buildStruct();
extern void ACCEL_readData();

#endif /* ACCELEROMETER_MANAGER_H_ */
