/*
 * kernel_user_pot.h
 *
 *  Created on: May 23, 2020
 *      Author: User
 */

#ifndef KERNEL_USER_POT_H_
#define KERNEL_USER_POT_H_

#define ADC_CHANNELS 1

extern volatile u16 ADC_Buf[ADC_CHANNELS];

extern float getShortcutOffset();

#endif /* KERNEL_USER_POT_H_ */
