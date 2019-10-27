/*
 * accelerometer_manager.h
 *
 *  Created on: 26 окт. 2019 г.
 *      Author: User
 */
#include "common.h"

#ifndef ACCELEROMETER_MANAGER_H_
#define ACCELEROMETER_MANAGER_H_



extern void ACCEL_Init(TIM_HandleTypeDef *htim, I2C_HandleTypeDef *hi2c);
extern void ACCEL_Task();
extern void ACCEL_PeriodElapsedCallback();

#endif /* ACCELEROMETER_MANAGER_H_ */
