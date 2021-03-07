#include "common.h"
#include "kernel.h"
#include "kernel_user_pot.h"
#include "stm32f1xx_hal_adc.h"

#define FREQ 3410

TIM_HandleTypeDef* mainTimer;
ADC_HandleTypeDef* hadc;
DMA_HandleTypeDef* hdma_adc;
TimerConf_t timerConf;
u16 shortStartValue;

u16 getShortStartValue()
{
	return (timerConf.Period / 2) * getShortcutOffset();
}

void kernel_init(TIM_HandleTypeDef* p_mainTimer, ADC_HandleTypeDef* p_hadc,
		DMA_HandleTypeDef* p_hdma_adc)
{
	mainTimer = p_mainTimer;
	hadc = p_hadc;
	hdma_adc = p_hdma_adc;

	calculatePeriodAndPrescaler(FREQ, &timerConf);
	mainTimer->Instance->PSC = timerConf.Prescaler;
	mainTimer->Instance->ARR = timerConf.Period;
	mainTimer->Instance->CCR1 = timerConf.Period / 2;

	shortStartValue = getShortStartValue();
	mainTimer->Instance->CCR2 = shortStartValue;

	HAL_ADC_Start_DMA(hadc, &ADC_Buf, ADC_CHANNELS);
}

void kernel_mainLoop()
{
	HAL_TIM_PWM_Start(mainTimer, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(mainTimer, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(mainTimer, TIM_CHANNEL_2);

	//HAL_TIM_OC_Start_IT(mainTimer, TIM_CHANNEL_2);
	HAL_TIM_Base_Start_IT(mainTimer);


	while(1){
		osDelay(1);

		//Start ADC if ready
		if (HAL_ADC_GetState(hadc) & HAL_ADC_STATE_READY != 0) {
			HAL_ADC_Start_DMA(hadc, &ADC_Buf, ADC_CHANNELS);
		}

	}
}

void TIM1_PeriodElapsedCallback(){
	//восстанавливаем ут€гиватель вниз (отключаем)
	GPIOB->BRR=GPIO_SHORT_STOP;
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim==mainTimer){

	}
}

void HAL_GPIO_EXTI_Callback(u16 pin){
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) {
		//ут€гиваем вниз
		GPIOB->BSRR=GPIO_SHORT_STOP;
	}
}
