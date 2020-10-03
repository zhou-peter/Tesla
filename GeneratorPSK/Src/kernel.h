#include "common.h"
#include "stm32f1xx_hal.h"


#ifndef __KERNEL_H
#define __KERNEL_H




#define GPIO_LED GPIOB
#define GPIO_LED_R GPIO_PIN_1
#define GPIO_LED_G GPIO_PIN_2

#define GPIO_BUTTON GPIOA
#define GPIO_BUTTON_WORK GPIO_PIN_0

typedef struct
{
	u16	accumulationCount:16;
	u16	pauseCount:16;
	u16 twoWavesCount:16;
	u16 springCount:16;	//can not be more than some ove aboves
	s16	phaseShift:16;
	bool reading:1; //for struct locking
	u16 tmp:15;
} Configuration_t;

typedef struct
{
	u16	shiftStart:16; // and pause start
	u16	shiftStop:16;
	u16	pauseStop:16;
	u16	twoWaveStart:16;
	u16 endIndex:16;
	bool updating:1; //for struct locking
	u16 tmp2:15;
} UsingConfiguration_t;

typedef enum
{
	KernelIdle,
	KernelFlatGenerating, //Green light
	KernelWorking			//Green blink
} KernelStates_t;

typedef struct
{
	KernelStates_t KernelState;
	bool zeroAngle;
} Env_t;

extern volatile Configuration_t configuration;
extern volatile UsingConfiguration_t usingConfig;





extern void Kernel_Init(TIM_HandleTypeDef* mainTimer, TIM_HandleTypeDef* halfTimer, ADC_HandleTypeDef* p_hadc,
		DMA_HandleTypeDef* p_hdma_adc, DAC_HandleTypeDef* hdac);
extern void Kernel_Task();
extern void UserDisplayTask();
extern void Kernel_TImer1Update();

#endif
