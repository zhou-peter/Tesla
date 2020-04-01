#include "common.h"
#include "kernel.h"

volatile Configuration_t configuration;
volatile UsingConfiguration_t* usingConfig;
volatile UsingConfiguration_t usingConfig1;
volatile UsingConfiguration_t usingConfig2;

Stage_t stage;
TimerConf_t timerConf;

TIM_HandleTypeDef* htim;

void Kernel_Init(TIM_HandleTypeDef* mainTimer) {
	//setup default values
	configuration.accumulationCount = MIN_ACCUMULATION_COUNT;
	configuration.shiftingCount = MIN_SHIFT_COUNT;
	configuration.phaseShiftPercentX10 = 0;
	configuration.twoWavesCount = MIN_TWO_WAVES_COUNT;

	htim = mainTimer;

	calculatePeriodAndPrescaler(62000, &timerConf);
	htim->Instance->PSC = timerConf.Prescaler;
	htim->Instance->ARR = timerConf.Period;
	htim->Instance->CCR1 = timerConf.Period / 2;
}

void buildConfig(volatile UsingConfiguration_t* config) {
	config->index = -1;
	config->shiftIndex = configuration.accumulationCount;
	config->twoWaveIndex = config->shiftIndex + configuration.shiftingCount;
	config->endIndex = config->twoWaveIndex + configuration.twoWavesCount;
	/*
	 if (config->endIndex % 2 != 0) {
	 config->endIndex++;
	 }
	 */
	//calculate phase shift
	//if set early, dec in ouputcompare (50)
	//if set later, inc in timer period
	config->phaseShift = timerConf.Period * configuration.phaseShiftPercentX10
			/ 1000;
	config->version = configuration.version;
}

void Kernel_Task() {

	buildConfig(&usingConfig1);
	usingConfig = &usingConfig1;

	HAL_TIM_OC_Start_IT(htim, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(htim);

	while (1) {
		nop();
	}

}

static inline void Kernel_Timer() {
	GPIOT->BRR = PIN_LOW;
	usingConfig->index++;

	switch (stage) {
	case Accumulating:
		if (usingConfig->index == usingConfig->shiftIndex) {
			stage = Shifting;
			goto Shifting_1;
		} else {
			GPIOT->BSRR = PIN_HI;
		}
		break;
		Shifting_1: case Shifting:
		GPIOT->BRR = PIN_HI;
		//actual shifting
		if (usingConfig->phaseShift == 0) {
			stage = AfterShifted;
		} else if (usingConfig->phaseShift > 0) {
			htim->Instance->CNT += usingConfig->phaseShift;
			stage = AfterShifted;
		}
		break;
	case AfterShifted:
		if (usingConfig->index == usingConfig->twoWaveIndex) {
			stage = TwoWaveGenerating;
			goto TwoWaveGen_1;
		}
		break;
		TwoWaveGen_1: case TwoWaveGenerating:
		GPIOT->BSRR = PIN_HI;
		break;
	case TwoWaveNotGenerating:
		GPIOT->BRR = PIN_HI;
		break;

	}
}

static inline void Kernel_HalfTimer() {
	GPIOT->BRR = PIN_HI;

	switch (stage) {
	case Shifting:
		if (usingConfig->index == usingConfig->shiftIndex
				&& usingConfig->phaseShift < 0) {
			htim->Instance->CNT += usingConfig->phaseShift;
			stage = AfterShifted;
		}
		break;
	case Accumulating:
		GPIOT->BSRR = PIN_LOW;
		break;
	case TwoWaveGenerating:
		GPIOT->BSRR = PIN_LOW;
		stage = TwoWaveNotGenerating;
		break;
	case TwoWaveNotGenerating:
		stage = TwoWaveGenerating;
		break;
	default:
		break;
	}

	if (usingConfig->index >= usingConfig->endIndex) {
		usingConfig->index = -1;
		stage = Accumulating;
	}
}

inline void Kernel_TIM_IRQHandler() {
	/* Capture compare 1 event */
	if (__HAL_TIM_GET_FLAG(htim, TIM_FLAG_CC1) != RESET
			&& __HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_CC1) != RESET) {
		__HAL_TIM_CLEAR_IT(htim, TIM_IT_CC1);
		//htim->Channel = HAL_TIM_ACTIVE_CHANNEL_1;
		/* Output compare event */

		Kernel_HalfTimer();

		//htim->Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;

	}
	/* TIM Update event */
	else if (__HAL_TIM_GET_FLAG(htim, TIM_FLAG_UPDATE) != RESET
			&& __HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_UPDATE) != RESET) {
		__HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
		Kernel_Timer();
	}
}

/*
 void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *atim) {
 if (atim == htim) {
 Kernel_Timer();
 }
 }

 void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *atim) {
 if (atim == htim) {
 Kernel_HalfTimer();
 }
 }
 */
