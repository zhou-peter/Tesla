/*
 * kernel_user_pot.h
 *
 *  Created on: May 23, 2020
 *      Author: User
 */

#ifndef KERNEL_USER_POT_H_
#define KERNEL_USER_POT_H_

#define ADC_CHANNELS 3

extern volatile u16 ADC_Buf[ADC_CHANNELS];

extern float up_getShortcutOffset();
extern float up_getShortcutLength();
extern float up_getPauseSize();

#endif /* KERNEL_USER_POT_H_ */
