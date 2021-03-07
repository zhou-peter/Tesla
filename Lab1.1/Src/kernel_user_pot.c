#include "common.h"
#include "kernel_user_pot.h"

volatile u16 ADC_Buf[1];
#define edge 20
volatile float offsetValue=(float)edge/(float)ADC_MAX;


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* p_hdma) {

	u16 value = ADC_Buf[0];
	//зона пол волны
	if (value >= edge && value < ADC_MAX - edge) {
		offsetValue = (float)value/(float)ADC_MAX;
	}

}

float getShortcutOffset()
{
	return offsetValue;
}
