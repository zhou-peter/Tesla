#include "common.h"
#include "stm32f1xx_hal.h"

#ifndef KERNEL_H_
#define KERNEL_H_

#define GPIO_SHORT_STOP GPIO_PIN_11


extern void kernel_init(TIM_HandleTypeDef* p_mainTimer, ADC_HandleTypeDef* p_hadc,
		DMA_HandleTypeDef* p_hdma_adc);
extern void kernel_mainLoop();

extern void TIM1_PeriodElapsedCallback();

#endif /* KERNEL_H_ */
