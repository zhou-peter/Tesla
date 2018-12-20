#include "kernel.h"
#include "stm32f1xx_hal_tim.h"



bool getBool(u8* ptr){
	bool result=FALSE;
	if (*ptr>0){
		result=TRUE;
	}
	return result;
}

u16 getU16(u8* ptr){
	u16 result=0;
	result |= *ptr;
	result |= (*(ptr+1))<<8;
	return result;
}

void resetTimerCounters(){
	htim4.Instance->CNT=0;
	htim2.Instance->CNT=0;
	htim1.Instance->CNT=0;
	htim3.Instance->CNT=0;
}
void stopTimers(){
  HAL_TIM_Base_Stop(&htim2);
  HAL_TIM_Base_Stop(&htim3);
  HAL_TIM_Base_Stop(&htim4);
  HAL_TIM_Base_Stop(&htim1);
}
void startTimers(){
  //HAL_TIM_Base_Start(&htim2);
  //HAL_TIM_Base_Start(&htim3);
  //HAL_TIM_Base_Start(&htim4);
  HAL_TIM_Base_Start(&htim1);
}


#define	FEATURE_CARRIER 		1
#define	FEATURE_BUNCH 			2
#define	FEATURE_GAP				3
#define	FEATURE_GAP_INDENT		6
#define	FEATURE_SKIP_HIGH		4
#define	FEATURE_SKIP_LOW		5

volatile void setFeatureState(u8 feature, bool state){
	stopTimers();
	resetTimerCounters();


	if (feature==FEATURE_CARRIER){
		State.F1=state;
		if (state==TRUE){
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
		}else{
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
		}
	}else if (feature==FEATURE_BUNCH){
		State.F2=state;
		if (state==TRUE){
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
		}else{
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
		}
	}else if (feature==FEATURE_GAP){
		State.F3=state;
		if (state==TRUE){
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
		}else{
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
		}
	}else if (feature==FEATURE_GAP_INDENT){
		State.F6=state;
		if (state==TRUE){
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
		}else{
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
		}
	}else if (feature==FEATURE_SKIP_HIGH){
		State.F4=state;
		if (state==TRUE){
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
		}else{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
		}
	}else if (feature==FEATURE_SKIP_LOW){
		State.F5=state;
		if (state==TRUE){
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
		}else{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
		}
	}

	if (State.F1==TRUE){
		startTimers();
	}
}




void packet_02_feature_change(u8* body, u16 bodySize){
	u8 num = *(body);
	bool enable = getBool(*(body+1));
	setFeatureState(num, enable);
}
void packet_04_timer_config(u8* body, u16 bodySize){
	stopTimers();
	resetTimerCounters();
	/*
Период несущей
Период проломов
Старт пролома
Стоп пролома
Период пачки
Скважность пачки
Отступ пролома в рабочей области пачки
Стоп пролома в рабочей области пачки
Отступ пропуск верхнее плечо
Стоп пропуска верхнего плеча
Отступ пропуск нижнее плечо
Стоп пропуска нижнее плеча
	*/
	u8* p=body;
	htim1.Instance->ARR=getU16(p);
	p+=2;
	htim1.Instance->CCR1=getU16(p);
	htim3.Instance->ARR=getU16(p);
	p+=2;
	htim3.Instance->CCR3=getU16(p);
	p+=2;
	htim3.Instance->CCR4=getU16(p);
	p+=2;
	htim2.Instance->ARR=getU16(p);
	htim4.Instance->ARR=getU16(p);
	p+=2;
	htim4.Instance->CCR2=getU16(p);
	p+=2;
	htim4.Instance->CCR3=getU16(p);
	p+=2;
	htim4.Instance->CCR4=getU16(p);
	p+=2;
	htim2.Instance->CCR3=getU16(p);
	p+=2;
	htim2.Instance->CCR4=getU16(p);
	p+=2;
	htim2.Instance->CCR1=getU16(p);
	p+=2;
	htim2.Instance->CCR2=getU16(p);

	startTimers();
}
