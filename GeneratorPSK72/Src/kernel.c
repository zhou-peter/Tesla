#include "common.h"
#include "kernel.h"

volatile Configuration_t configuration;
volatile UsingConfiguration_t* usingConfig;
volatile UsingConfiguration_t usingConfig1;
//ADC1 IN1 halfwave
//IN2 shift area
//IN3 twowave area
//IN4 phase
volatile u16 ADC_Buf[4];
#define ADC_MAX 4096
#define ADC_MIDDLE 2048
//максимальный сдвиг - это 90 градусов или 1/4 периода
s32 maxShift;
Stage_t stage;
s32 index;
TimerConf_t timerConf;

TIM_HandleTypeDef* htim;
ADC_HandleTypeDef* hadc;
DMA_HandleTypeDef* hdma_adc;

void Kernel_Init(TIM_HandleTypeDef* mainTimer, ADC_HandleTypeDef* p_hadc,
		DMA_HandleTypeDef* p_hdma_adc) {

	htim = mainTimer;
	hadc = p_hadc;
	hdma_adc = p_hdma_adc;

	//setup default values
	configuration.accumulationCount = MIN_ACCUMULATION_COUNT;
	configuration.shiftingCount = MIN_SHIFT_COUNT;
	configuration.phaseShift = 0;
	configuration.twoWavesCount = MIN_TWO_WAVES_COUNT;

	calculatePeriodAndPrescaler(528000, &timerConf);
	maxShift = (timerConf.Period / 4) - 5;
	htim->Instance->PSC = timerConf.Prescaler;
	htim->Instance->ARR = timerConf.Period;
	htim->Instance->CCR1 = timerConf.Period / 2;

	HAL_ADC_Start_DMA(hadc, &ADC_Buf, 4);
}

void buildConfig(volatile UsingConfiguration_t* config) {
	config->shiftIndex = configuration.accumulationCount;
	config->twoWaveIndex = config->shiftIndex + configuration.shiftingCount;
	config->endIndex = config->twoWaveIndex + configuration.twoWavesCount;

	//calculate phase shift
	//if set early, dec in ouputcompare (50)
	//if set later, inc in timer period
	config->phaseShift = configuration.phaseShift;

	config->lowPowerIndex = config->twoWaveIndex + configuration.twoWavesCount / 2;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* p_hdma) {

	u16 value = ADC_Buf[0];
	//зона пол волны
	if (value >= 0 && value < ADC_MAX) {
		configuration.accumulationCount = MIN_ACCUMULATION_COUNT
				+ ((MAX_ACCUMULATION_COUNT - MIN_ACCUMULATION_COUNT) * value
						/ ADC_MAX);
	}

	//зона паузы
	value = ADC_Buf[1];
	if (value >= 0 && value < ADC_MAX) {
		configuration.shiftingCount = MIN_SHIFT_COUNT
				+ ((MAX_SHIFT_COUNT - MIN_SHIFT_COUNT) * value / ADC_MAX);
	}

	//зона двухволнового режима
	value = ADC_Buf[2];
	if (value >= 0 && value < ADC_MAX) {
		configuration.twoWavesCount =
				MIN_TWO_WAVES_COUNT
						+ ((MAX_TWO_WAVES_COUNT - MIN_TWO_WAVES_COUNT) * value
								/ ADC_MAX);
	}

	//фаза
	value = ADC_Buf[3];
	if (value >= 0 && value < ADC_MAX) {
		if (value >= ADC_MIDDLE) {
			value -= ADC_MIDDLE;
			//умножаем maxShift на понижающий коэффициент
			configuration.phaseShift = maxShift * value / ADC_MIDDLE;
			if (configuration.phaseShift < 10) {
				GPIOC->BRR = GPIO_PIN_13;
			} else {
				GPIOC->BSRR = GPIO_PIN_13;
			}
		} else {
			value = ADC_MIDDLE - value;
			configuration.phaseShift = (-1) * maxShift * value / ADC_MIDDLE;
			if (configuration.phaseShift > -10) {
				GPIOC->BRR = GPIO_PIN_13;
			} else {
				GPIOC->BSRR = GPIO_PIN_13;
			}
		}
	}

}

void Kernel_Task() {

	buildConfig(&usingConfig1);
	usingConfig = &usingConfig1;

	GPIOT->BSRR = PIN_HI;
	GPIOT->BSRR = PIN_LOW;

	HAL_TIM_OC_Start_IT(htim, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(htim);

	while (1) {
		nop();
	}

}

void Kernel_Timer() {
	GPIOT->BRR = PIN_LOW;
	index++;

	switch (stage) {
	case Accumulating:
		if (index == usingConfig->shiftIndex) {
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
		if (index == usingConfig->twoWaveIndex) {
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

void Kernel_HalfTimer() {
	GPIOT->BRR = PIN_HI;

	switch (stage) {
	case Shifting:
		if (index == usingConfig->shiftIndex && usingConfig->phaseShift < 0) {
			htim->Instance->CNT += usingConfig->phaseShift;
			stage = AfterShifted;
		}
		break;
	case AfterShifted:
		//set high voltage power supply
		GPIOT->BRR = PIN_LOW_PWR;
		GPIOT->BSRR = PIN_HI_PWR;
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

	if (index >= usingConfig->lowPowerIndex) {
		//set low voltage power supply
		GPIOT->BRR = PIN_HI_PWR;
		GPIOT->BSRR = PIN_LOW_PWR;
	}
	if (index >= usingConfig->endIndex) {
		index = -1;
		stage = Accumulating;
		//сетапим новую конфигурацию
		buildConfig(usingConfig);
	}
}

void Kernel_TIM_IRQHandler() {
	/* Capture compare 1 event */
	if (__HAL_TIM_GET_FLAG(htim, TIM_FLAG_CC1) != RESET
			&& __HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_CC1) != RESET) {
		__HAL_TIM_CLEAR_IT(htim, TIM_IT_CC1);

		/* Output compare event */
		Kernel_HalfTimer();
	}
	/* TIM Update event */
	else if (__HAL_TIM_GET_FLAG(htim, TIM_FLAG_UPDATE) != RESET
			&& __HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_UPDATE) != RESET) {
		__HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
		Kernel_Timer();
	}
}

