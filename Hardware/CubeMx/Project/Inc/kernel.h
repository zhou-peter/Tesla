#include "common.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#ifndef __KERNEL_H
#define __KERNEL_H

extern void packet_02_feature_change(u8* body, u16 bodySize);
extern void packet_04_timer_config(u8* body, u16 bodySize);
extern void packet_06_search(u8* body, u16 bodySize);
extern void packet_08_search_stop(u8* body, u16 bodySize);
extern void packet_0A_just_generate(u8* body, u16 bodySize);


extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim16;
#endif
