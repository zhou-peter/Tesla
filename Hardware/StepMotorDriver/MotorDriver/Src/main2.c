/*
 * main2.c
 *
 *  Created on: 8 июл. 2018 г.
 *      Author: user
 */
#include "stm32f1xx_hal.h"
#include "common.h"
#include "main.h"
#include "main2.h"

volatile u16 ADC_Buf[1];
volatile Env_t Env;

void initMain(){
	//100√ц
	Env.freq=10000;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

	__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE );
	__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CC1 );
	HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_1);
}

extern void adcMeasure();
extern void checkSpeed();
extern void resetSteps();
void loopMain(){
	checkState();
	while (1)
	{
		int shortCounter=10;
		while(shortCounter--){
			//nop
			HAL_Delay(10);
		}
		adcMeasure();
		HAL_Delay(10);
		checkSpeed();
	}
}

void timerUpdate(){
    u32 freq=(u32)((float)Env.freq/100.0F);
    if (freq%2!=0)freq--;//четна€ частота
    u16 prescaler=1;
    while ((SystemCoreClock/(freq*prescaler))>MAX_PERIOD) {
    	prescaler++;
    }
    int clock=SystemCoreClock/prescaler;
    u16 period=clock/freq;

/*
	  htim3.Init.Prescaler = prescaler;
	  htim3.Init.Period = period;
	  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	  HAL_TIM_Base_Init(&htim3);
*/

	  htim3.Instance->PSC=prescaler;
	  htim3.Instance->ARR= period;
	  htim3.Instance->CCR1=period/2;
}
//get enable pin
//direction pin
//enable or disable timer
void checkState(){
	//GPIO_PinState pa0 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	//GPIO_PinState pa1 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);

	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)){
		if (Env.Enabled==FALSE){
			HAL_TIM_Base_Start_IT(&htim3);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
			Env.Enabled=TRUE;
		}
	}else{
		if (Env.Enabled==TRUE){
			HAL_TIM_Base_Stop_IT(&htim3);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
			Env.Enabled=FALSE;
			resetSteps();
		}
	}


	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)){//GPIO_PIN_SET
		Env.Direction=CW;
	}else{
		Env.Direction=CCW;
	}
}

void checkSpeed(){
	u16 max=0x3FF;
	u16 current=ADC_Buf[0];
	u16 delta=35000;//from 150Hz to 500Hz
	u32 addValue=current*delta/max;
	u32 freq=15000+addValue;
	s32 differ=freq-Env.freq;
	if (differ<0)differ*=(-1);
	if (differ>200){
		Env.freq=freq;
		timerUpdate();
	}
}
void resetSteps(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_13, GPIO_PIN_RESET);
}


void adcMeasure()
{
	HAL_ADC_Start_IT(&hadc1);
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	u32 value = HAL_ADC_GetValue(hadc);
	ADC_Buf[0]=(u16)value;
	HAL_ADC_Stop_IT(hadc);
}

void HAL_GPIO_EXTI_Callback(u16 pin){
	checkState();
}

#define increment 0.5F
volatile float bank=0;


void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){
	resetSteps();
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if (Env.Enabled==TRUE){
		//next step logic
		if (Env.Direction==CW){
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

			if (Env.CurrentStep==3){
				Env.CurrentStep=0;
			}else{
				Env.CurrentStep++;
			}
		}else{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);

			if (Env.CurrentStep==0){
				Env.CurrentStep=3;
			}else{
				Env.CurrentStep--;
			}
		}

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);


		bank+=increment;
		if (bank>=1.0F){
			bank-=1.0F;
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
		}
	}
	else{
		//reset all bits
		resetSteps();
		bank=0;
	}
}
