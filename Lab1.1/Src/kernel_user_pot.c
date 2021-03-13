#include "common.h"
#include "kernel_user_pot.h"

volatile u16 ADC_Buf[ADC_CHANNELS];
#define edge 20
volatile float offsetValue=(float)edge/(float)ADC_MAX;
volatile float shortLength=(float)edge/(float)ADC_MAX;
volatile float pauseSize=(float)edge/(float)ADC_MAX;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* p_hdma) {

	u16 value = ADC_Buf[0];
	//зона пол волны
	if (value >= edge && value < ADC_MAX - edge) {
		offsetValue = (float)value/(float)ADC_MAX;
	}
	value = ADC_Buf[1];
	if (value >= edge && value < ADC_MAX - edge) {
		shortLength = (float)value/(float)ADC_MAX;
	}
	value = ADC_Buf[2];
	if (value >= edge && value < ADC_MAX - edge) {
		pauseSize = (float)value/(float)ADC_MAX;
	}
}

float up_getShortcutOffset()
{
	return offsetValue;
}
float up_getShortcutLength()
{
	return shortLength;
}
float up_getPauseSize()
{
	return pauseSize;
}
