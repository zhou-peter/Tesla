#include "common.h"
#include "stm32f1xx_hal.h"


#ifndef __KERNEL_H
#define __KERNEL_H

#define MIN_ACCUMULATION_COUNT 2
#define MAX_ACCUMULATION_COUNT 1000

#define MIN_PAUSE_COUNT 1
#define MAX_PAUSE_COUNT 100

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
	u16	pauseCount:16;
	u16 twoWavesCount:16;
	u16 springCount:16;	//can not be more than some ove aboves
	s16	phaseShift:16; //25% == 90deg, 250 == 25%
	u16 tmp:16;
} Configuration_t;

typedef struct
{
	u16	shiftStart:16; // and pause start
	u16	shiftStop:16;
	u16	pauseStop:16;
	u16	twoWaveStart:16;
	u16 endIndex:16;
	bool updating:1;
} UsingConfiguration_t;

extern volatile Configuration_t configuration;
extern volatile UsingConfiguration_t usingConfig1;
extern volatile UsingConfiguration_t usingConfig2;
extern volatile UsingConfiguration_t* inModifyConfig;


extern void Kernel_Init(TIM_HandleTypeDef* mainTimer, TIM_HandleTypeDef* halfTimer, ADC_HandleTypeDef* p_hadc,
		DMA_HandleTypeDef* p_hdma_adc, DAC_HandleTypeDef* p_hdac);
extern void Kernel_Task();

#endif
