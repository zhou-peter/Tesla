/*
 * main2.h
 *
 *  Created on: 8 èþë. 2018 ã.
 *      Author: user
 */

#ifndef MAIN2_H_
#define MAIN2_H_
#include "common.h"
#include "main.h"

#define MAX_PERIOD 0xFFFF
extern TIM_HandleTypeDef htim7;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

extern void initMain();
extern void loopMain();



#endif /* MAIN2_H_ */
