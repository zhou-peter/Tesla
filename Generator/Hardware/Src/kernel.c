#include "kernel.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_tim_ex.h"
#include "timers.h"
#include "common.h"

volatile State_t State;

bool getBool(u8* ptr) {
	bool result = FALSE;
	if (*ptr > 0) {
		result = TRUE;
	}
	return result;
}

u16 getU16(u8* ptr) {
	u16 result = 0;
	result |= *ptr;
	result |= (*(ptr + 1)) << 8;
	return result;
}

void resetTimerCounters() {
	//htim1.Instance->CNT = 0;
	//htim3.Instance->CNT = 0;
}
void stopTimers() {
	//HAL_TIM_Base_Stop(&htim1);
	//HAL_TIM_Base_Stop(&htim3);
}
void startTimers() {
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Start(&htim1);
}

#define	FEATURE_CARRIER 		1
#define FEATURE_PWR				10

volatile void setFeatureState(u8 feature, bool state) {
	stopTimers();

	if (feature == FEATURE_CARRIER) {
		u16 period = htim1.Instance->ARR;
		if (period < 2) {
			return;
		}
		State.F1 = state;
		if (state == TRUE) {
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
		} else {
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
		}
	} else if (feature == FEATURE_PWR) {
		State.F10 = state;
		if (state == TRUE) {
			//HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
			//HAL_TIMEx_PWMN_Start(&htim16, TIM_CHANNEL_1);
		} else {
			//HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);
			//HAL_TIMEx_PWMN_Stop(&htim16, TIM_CHANNEL_1);
		}
	}

	/*if (State.F1 == TRUE) {
		startTimers();
	}*/

	stopTimers();
	resetTimerCounters();
	startTimers();
}

void packet_02_feature_change(u8* body, u16 bodySize) {
	u8 num = *(body);
	bool enable = getBool(*(body + 1));
	setFeatureState(num, enable);
}

volatile u16 steps = 12;

void packet_0A_just_generate(u8* body, u16 bodySize) {
	u8* p = body;
	u16 period = getU16(p);
	p += 2;
	u16 duty = getU16(p);
	p += 2;
	u8 feature = *p;

	if (feature == FEATURE_CARRIER) {
		htim1.Instance->ARR = period;
		htim1.Instance->CCR1 = duty;
		u16 period3=period*steps;
		period3-=period/8;//45deg
		htim3.Instance->ARR=period3;
	} else if (feature == FEATURE_PWR) {
		//htim16.Instance->ARR=period;
		//htim16.Instance->CCR1=duty;
	}
}

void KERNEL_Init(){
	/*__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	HAL_NVIC_SetPriority(TIM3_IRQn, 1, 2);
	HAL_TIM_Base_Start_IT(&htim3);*/
}

volatile u32 counter=0;

void KERNEL_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM3) {
		/*HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
		HAL_TIM_Base_Stop(&htim1);
		HAL_TIM_Base_Stop(&htim3);
		htim1.Instance->CNT=0;
		htim3.Instance->CNT=0;

		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
		HAL_TIM_Base_Start(&htim1);*/
		htim1.Instance->CNT = 0;
		//resetTimerCounters();
		//counter++;
	}

}




