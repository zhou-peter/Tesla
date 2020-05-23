#include "common.h"
#include "kernel.h"
#include "kernel_user_pot.h"
#include "stm32f1xx_hal_dac.h"



TIM_HandleTypeDef* htim;
TIM_HandleTypeDef* htimHalfWave;
ADC_HandleTypeDef* hadc;
DMA_HandleTypeDef* hdma_adc;
DAC_HandleTypeDef* hdac;

volatile Configuration_t configuration;
volatile UsingConfiguration_t usingConfig1;
volatile UsingConfiguration_t usingConfig2;
volatile UsingConfiguration_t* inModifyConfig;




volatile u32 DAC_Regular;
volatile u32 DAC_Shift;




void Kernel_Init(TIM_HandleTypeDef* mainTimer, TIM_HandleTypeDef* halfTimer, ADC_HandleTypeDef* p_hadc,
		DMA_HandleTypeDef* p_hdma_adc, DAC_HandleTypeDef* p_hdac) {
	htim = mainTimer;
	htimHalfWave = halfTimer;
	hadc = p_hadc;
	hdma_adc = p_hdma_adc;
	hdac=p_hdac;

	//setup default values
	configuration.accumulationCount = MIN_ACCUMULATION_COUNT;
	configuration.pauseCount = MIN_PAUSE_COUNT;
	configuration.phaseShift = 0;
	configuration.twoWavesCount = MIN_TWO_WAVES_COUNT;



	htimHalfWave->Instance->ARR = 2;
	htimHalfWave->Instance->CCR4 = 1;


	DAC_Regular = 3000;
	DAC_Shift = 3500;

	HAL_TIM_Base_Start_IT(htim);
	HAL_ADC_Start_DMA(hadc, &ADC_Buf, 4);
	HAL_DAC_Start_DMA(hdac, DAC_CHANNEL_1, &DAC_Regular, 1, DAC_ALIGN_12B_R);


	HAL_TIM_Base_Start(htim);
	HAL_TIM_Base_Start(htimHalfWave);
}



void Kernel_Task() {



	while (1) {
		osDelay(100);
	}

}

