/*
 * accelerometer_manager.h
 *
 *  Created on: 26 ���. 2019 �.
 *      Author: User
 */
#include "common.h"

#ifndef ACCELEROMETER_MANAGER_H_
#define ACCELEROMETER_MANAGER_H_


typedef enum
{
	AccelIdle,
	AccelConfig,
	AccelReady,		//sleeping
	AccelShouldRequest,
	AccelDataRetriving,//reading data
	AccelDataRetrived,//moved from I2C to tmp buf
	AccelDataReady, //accelerometer manger will stay here until
					//somebody will change state to AccelReady
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
	u32 	ticks:32;
	u16		tmp5:16;
	s16		x:16;
	s16		y:16;
	s16		z:16;
} AccelData_t;

extern TaskHandle_t gHandle;
extern volatile AccelState_t 	AccelState;
extern volatile AccelData_t 	AccelData;

extern void ACCEL_Init(TIM_HandleTypeDef *htim, I2C_HandleTypeDef *hi2c, TaskHandle_t taskHandle);
extern void ACCEL_Task();
extern void Accelerometer_Config(I2C_HandleTypeDef *hi2c, TaskHandle_t taskHandle);
extern void ACCEL_PeriodElapsedCallback();
extern void ACCEL_buildStruct();
extern void ACCEL_readData();

#endif /* ACCELEROMETER_MANAGER_H_ */