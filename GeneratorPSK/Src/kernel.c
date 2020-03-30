#include "common.h"
#include "kernel.h"

volatile Configuration_t configuration;
volatile UsingConfiguration_t* usingConfig;
volatile UsingConfiguration_t usingConfig1;
volatile UsingConfiguration_t usingConfig2;

TIM_HandleTypeDef* htim;

void Kernel_Init(TIM_HandleTypeDef* mainTimer) {
	//setup default values
	configuration.accumulationCount = MIN_ACCUMULATION_COUNT;
	configuration.shiftingCount = MIN_SHIFT_COUNT;
	configuration.phaseShiftPercentX10 = -100;
	configuration.twoWavesCount = MIN_TWO_WAVES_COUNT;

	htim = mainTimer;

	TimerConf_t timerConf = calculatePeriodAndPrescaler(62000);
	htim->Instance->PSC = timerConf.Prescaler;
	htim->Instance->ARR = timerConf.Period;

}

void buildConfig(UsingConfiguration_t* config) {
	config->index = 0;
	config->shiftIndex = configuration.accumulationCount;
	config->twoWaveIndex = config->shiftIndex + configuration.shiftingCount;
	config->endIndex = config->twoWaveIndex + configuration.twoWavesCount;
	//ensure semicolon
	if (config->endIndex % 2 == 0) {
		config->endIndex--;
	}
	//calculate phase shift
	//if set early, dec in ouputcompare (50)
	//if set later, inc in timer period
	config->phaseShift = htim->Instance->ARR * configuration.phaseShiftPercentX10 / 1000;
	config->version = configuration.version;
}

void Kernel_Task() {
	HAL_TIM_Base_Start_IT(htim);
	buildConfig(&usingConfig1);
	usingConfig = &usingConfig1;

	while (1) {
		nop();
	}

}

void Kernel_Timer() {

}

