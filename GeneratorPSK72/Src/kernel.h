#include "common.h"
#include "stm32f1xx_hal.h"


#ifndef __KERNEL_H
#define __KERNEL_H

#define MIN_ACCUMULATION_COUNT 2
#define MAX_ACCUMULATION_COUNT 1000

#define MIN_SHIFT_COUNT 1
#define MAX_SHIFT_COUNT 100

//-90deg
#define MIN_PHASE -250
//+90deg
#define MAX_PHASE 250

#define MIN_TWO_WAVES_COUNT 3
#define MAX_TWO_WAVES_COUNT 100


#define GPIOT GPIOA
#define PIN_HI GPIO_PIN_7
#define PIN_LOW GPIO_PIN_8

typedef struct
{
	u16	accumulationCount:16;
	u16	shiftingCount:16;
	u16 twoWavesCount:16;
	u16 springCount:16;	//can not be more than some ove aboves
	s16	phaseShiftPercentX10:16; //25% == 90deg, 250 == 25%
	u16 version:16;
} Configuration_t;

typedef struct
{
	s32 index:16;
	u16 version:16;
	u16	shiftIndex:16;
	s16	phaseShift:16;
	u16	twoWaveIndex:16;
	u16 endIndex:16;
	u16 springIndex:16;

} UsingConfiguration_t;

typedef enum {
	Accumulating = 0,
	Shifting = 1,
	AfterShifted = 2,
	TwoWaveGenerating = 3,
	TwoWaveNotGenerating = 4
} Stage_t;

extern void Kernel_Init(TIM_HandleTypeDef* mainTimer);
extern void Kernel_Task();
extern void Kernel_TIM_IRQHandler();
#endif
