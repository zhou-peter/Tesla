#include "kernel.h"



volatile u16 ADC_Buf[4];

//максимальный сдвиг - это 90 градусов или 1/4 периода
#define MAX_SHIFT 88

//ADC1 IN1 halfwave
//IN2 shift area
//IN3 twowave area
//IN4 phase


void buildConfig(volatile UsingConfiguration_t* config) {

	config->updating = TRUE;
	config->shiftStart = configuration.accumulationCount;
	config->shiftStop = config->shiftStart + 1;
	config->pauseStop = config->shiftStop + configuration.pauseCount;
	config->twoWaveStart = config->shiftStop;
	config->endIndex = config->twoWaveStart + configuration.twoWavesCount;
	config->updating = FALSE;
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
		configuration.pauseCount = MIN_PAUSE_COUNT
				+ ((MAX_PAUSE_COUNT - MIN_PAUSE_COUNT) * value / ADC_MAX);
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
			//умножаем MAX_SHIFT на понижающий коэффициент
			configuration.phaseShift = MAX_SHIFT * value / ADC_MIDDLE;
			if (configuration.phaseShift < 10) {
				GPIOC->BRR = GPIO_PIN_13;
			} else {
				GPIOC->BSRR = GPIO_PIN_13;
			}
		} else {
			value = ADC_MIDDLE - value;
			configuration.phaseShift = (-1) * MAX_SHIFT * value / ADC_MIDDLE;
			if (configuration.phaseShift > -10) {
				GPIOC->BRR = GPIO_PIN_13;
			} else {
				GPIOC->BSRR = GPIO_PIN_13;
			}
		}
	}

}
