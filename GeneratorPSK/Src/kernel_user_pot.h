/*
 * kernel_user_pot.h
 *
 *  Created on: May 23, 2020
 *      Author: User
 */

#ifndef KERNEL_USER_POT_H_
#define KERNEL_USER_POT_H_

extern volatile u16 ADC_Buf[5];

#define MIN_ACCUMULATION_COUNT 2
#define MAX_ACCUMULATION_COUNT 1000

#define MIN_PAUSE_COUNT 1
#define MAX_PAUSE_COUNT 1000

#define MIN_TWO_WAVES_COUNT 3
#define MAX_TWO_WAVES_COUNT 1000

//максимальный сдвиг - это 90 градусов или 1/4 периода
#define MAX_SHIFT 90

#endif /* KERNEL_USER_POT_H_ */
