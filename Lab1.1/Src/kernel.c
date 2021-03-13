#include "common.h"
#include "kernel.h"
#include "kernel_user_pot.h"
#include "stm32f1xx_hal_adc.h"

#define FREQ 3410
#define PAUSE_MAX 50

TIM_HandleTypeDef* mainTimer;
TIM_HandleTypeDef* pauseTimer;
ADC_HandleTypeDef* hadc;
DMA_HandleTypeDef* hdma_adc;
TimerConf_t timerConf;
volatile u16 shortStartValue;
volatile u16 shortStopValue;
volatile u16 pausePeriod;

u16 getShortStartValue()
{
	return (timerConf.Period / 2) * up_getShortcutOffset();
}
u16 getShortLength()
{
	return (timerConf.Period / 2) * up_getShortcutLength();
}
u16 getPauseSize()
{
	u16 result = PAUSE_MAX * up_getPauseSize();
	if (result==0){
		result=1;
	}
	return result;
}


void kernel_init(TIM_HandleTypeDef* p_mainTimer, TIM_HandleTypeDef* p_pauseTimer,
		ADC_HandleTypeDef* p_hadc,
		DMA_HandleTypeDef* p_hdma_adc)
{
	mainTimer = p_mainTimer;
	pauseTimer = p_pauseTimer;
	hadc = p_hadc;
	hdma_adc = p_hdma_adc;

	calculatePeriodAndPrescaler(FREQ, &timerConf);

	shortStartValue = getShortStartValue();
	shortStopValue = shortStartValue + getShortLength();
	pausePeriod = getPauseSize();

	mainTimer->Instance->PSC = timerConf.Prescaler;
	mainTimer->Instance->ARR = timerConf.Period;
	mainTimer->Instance->CCR1 = timerConf.Period / 2;
	mainTimer->Instance->CCR2 = shortStartValue;
	mainTimer->Instance->CCR3 = shortStopValue;

	pauseTimer->Instance->ARR = pausePeriod;
	pauseTimer->Instance->CCR2 = 1;

	HAL_ADC_Start_DMA(hadc, &ADC_Buf, ADC_CHANNELS);
}

void kernel_mainLoop()
{
	HAL_TIM_PWM_Start(pauseTimer, TIM_CHANNEL_2);
	HAL_TIM_Base_Start_IT(pauseTimer);

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

		shortStartValue = getShortStartValue();
		shortStopValue = shortStartValue + getShortLength();

		u16 newPauseSize = getPauseSize();
		if (newPauseSize != pausePeriod)
		{
			pausePeriod = newPauseSize;
			pauseTimer->Instance->ARR = pausePeriod;
		}
	}
}

void TIM1_PeriodElapsedCallback(){
	//восстанавливаем утягиватель вниз (отключаем)
	GPIOB->BSRR=GPIO_SHORT_STOP;
	//обновляем время срабатывания коротилки
	mainTimer->Instance->CCR2 = shortStartValue;
	mainTimer->Instance->CCR3 = shortStopValue;
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim==mainTimer){

	}
}

void HAL_GPIO_EXTI_Callback(u16 pin){
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)==GPIO_PIN_SET) {
		//утягиваем вниз
		GPIOB->BRR=GPIO_SHORT_STOP;
	}
}
