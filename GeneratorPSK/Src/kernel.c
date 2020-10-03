#include "common.h"
#include "kernel.h"
#include "kernel_user_pot.h"
#include "stm32f1xx_hal_dac.h"

#define DAC_BOTTOM	1570
#define DAC_MIDDLE	1600
#define DAC_TOP		2000

TIM_HandleTypeDef* htim;
TIM_HandleTypeDef* htimHalfWave;
ADC_HandleTypeDef* hadc;
DMA_HandleTypeDef* hdma_adc;
DAC_HandleTypeDef* hdacPtr;

volatile Configuration_t configuration;
volatile UsingConfiguration_t usingConfig;

volatile Env_t Env;

volatile u32 DAC_Regular;
volatile u32 DAC_Shift;

void Kernel_Init(TIM_HandleTypeDef* mainTimer, TIM_HandleTypeDef* halfTimer,
		ADC_HandleTypeDef* p_hadc, DMA_HandleTypeDef* p_hdma_adc,
		DAC_HandleTypeDef* p_hdac) {
	htim = mainTimer;
	htimHalfWave = halfTimer;
	hadc = p_hadc;
	hdma_adc = p_hdma_adc;
	hdacPtr = p_hdac;

	//setup default values
	configuration.accumulationCount = MIN_ACCUMULATION_COUNT;
	configuration.pauseCount = MIN_PAUSE_COUNT;
	configuration.phaseShift = 0;
	configuration.twoWavesCount = MIN_TWO_WAVES_COUNT;

	htimHalfWave->Instance->ARR = 2;
	htimHalfWave->Instance->CCR4 = 1;

	DAC_Regular = DAC_MIDDLE;
	DAC_Shift = DAC_TOP;

	HAL_DAC_SetValue(hdacPtr, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_Regular);
	HAL_DAC_Start(hdacPtr, DAC_CHANNEL_1);

	HAL_DAC_SetValue(hdacPtr, DAC_CHANNEL_2, DAC_ALIGN_12B_R, DAC_Shift);
	HAL_DAC_Start(hdacPtr, DAC_CHANNEL_2);

	HAL_ADC_Start_DMA(hadc, &ADC_Buf, 5);

	/*
	 htim->Instance->ARR=7;
	 htim->Instance->CCR1=3;//shift start
	 htim->Instance->CCR2=4;//shift stop
	 htim->Instance->CCR3=4;//pause stop
	 htim->Instance->CCR4=6;//hv stop

	 HAL_TIM_Base_Start(htim);
	 HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
	 HAL_TIM_PWM_Start(htim, TIM_CHANNEL_2);
	 HAL_TIM_PWM_Start(htim, TIM_CHANNEL_3);
	 HAL_TIM_PWM_Start(htim, TIM_CHANNEL_4);

	 //таймер 2 канал 3
	 HAL_TIM_Base_Start(htimHalfWave);
	 HAL_TIM_PWM_Start(htimHalfWave, TIM_CHANNEL_3);
	 */

	//74HC74 SYNC enable
}

void buildConfig() {
	usingConfig.shiftStart = configuration.accumulationCount;
	usingConfig.shiftStop = usingConfig.shiftStart + 1;
	usingConfig.pauseStop = usingConfig.shiftStart + configuration.pauseCount;
	usingConfig.twoWaveStart = usingConfig.shiftStop;
	usingConfig.endIndex = usingConfig.twoWaveStart
			+ configuration.twoWavesCount;
	if (configuration.phaseShift >= 0) {
		u16 delta = DAC_TOP - DAC_MIDDLE;
		//delta : 90deg
		//  x   : phaseShift
		usingConfig.phaseAngle = DAC_MIDDLE
				+ delta * configuration.phaseShift / MAX_SHIFT;
	} else {
		u16 delta = DAC_MIDDLE - DAC_BOTTOM;
		usingConfig.phaseAngle = DAC_MIDDLE
				+ delta * configuration.phaseShift / MAX_SHIFT; // здесь вычитание!
	}
}

void buildConfigSafe() {
	//building structure from user pot
	configuration.reading = TRUE;
	usingConfig.updating = TRUE;
	buildConfig();
	configuration.reading = FALSE;
	usingConfig.updating = FALSE;
}

void moveConfigToPerepheral() {
	htim->Instance->ARR = usingConfig.endIndex;
	htim->Instance->CCR1 = usingConfig.shiftStart; //shift start
	htim->Instance->CCR2 = usingConfig.shiftStop; //shift stop
	htim->Instance->CCR3 = usingConfig.pauseStop; //pause stop
	//htim->Instance->CCR4=6;//hv stop

	//обновляем угол (основная частота не меняется)
	DAC_Shift=usingConfig.phaseAngle;
	HAL_DAC_SetValue(hdacPtr, DAC_CHANNEL_2, DAC_ALIGN_12B_R, DAC_Shift);
	HAL_DAC_Start(hdacPtr, DAC_CHANNEL_2);
}

void enableGeneration(bool withPWM) {
	HAL_TIM_Base_Start_IT(htim);
	if (withPWM) {
		HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(htim, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(htim, TIM_CHANNEL_3);
		HAL_TIM_PWM_Start(htim, TIM_CHANNEL_4);
	} else {
		HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_2);
		HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_3);
		HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_4);
	}
}

void Kernel_Task() {

	buildConfigSafe();
	moveConfigToPerepheral();
	enableGeneration(FALSE);

	while (1) {
		//Start ADC if ready
		if (HAL_ADC_GetState(hadc) & HAL_ADC_STATE_READY != 0) {
			HAL_ADC_Start_DMA(hadc, &ADC_Buf, 5);
		}
		osDelay(2);

		buildConfigSafe();
		if (Env.periodElapsed == TRUE) {
			moveConfigToPerepheral();
			Env.periodElapsed = FALSE;
		}

		s16 angle =
				configuration.phaseShift >= 0 ?
						configuration.phaseShift :
						configuration.phaseShift * (-1);

		//определяем фазу
		if (angle < 10) {
			Env.zeroAngle = TRUE;
		} else {
			Env.zeroAngle = FALSE;
		}

		//смотрим на кнопку
		GPIO_PinState buttonState = HAL_GPIO_ReadPin(GPIO_BUTTON,
		GPIO_BUTTON_WORK);

		if (Env.KernelState == KernelFlatGenerating
				&& buttonState == GPIO_PIN_SET) {
			moveConfigToPerepheral();
			enableGeneration(TRUE);
			Env.KernelState = KernelWorking;
		}

		if (Env.KernelState == KernelWorking && buttonState == GPIO_PIN_RESET) {
			enableGeneration(FALSE);
			Env.KernelState = KernelFlatGenerating;
		}

	}

}

void TIM1_PeriodElapsedCallback() {
	Env.periodElapsed = TRUE;
}

void UserDisplayTask() {
	bool blinkLight = TRUE;
	while (1) {
		//красный светодиод
		//если угол меньше 10 по модулю, то зажигаем красный диод
		if (Env.zeroAngle == TRUE) {
			GPIO_LED->BSRR = GPIO_LED_R;
		} else {
			GPIO_LED->BRR = GPIO_LED_R;
		}

		//зеленый светодиод
		if (Env.KernelState == KernelFlatGenerating) {
			GPIO_LED->BSRR = GPIO_LED_G;
		} else if (Env.KernelState == KernelWorking) {
			if (blinkLight == TRUE) {
				GPIO_LED->BRR = GPIO_LED_G;
				blinkLight = FALSE;
			} else {
				GPIO_LED->BSRR = GPIO_LED_G;
				blinkLight = TRUE;
			}
		}
		osDelay(500);
	}
}

