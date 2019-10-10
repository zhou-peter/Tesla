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

#define htimMain htim15
#define GPIOM GPIOB
volatile u16 ADC_Buf[2];
volatile Env_t Env;

extern void adcComplete(DMA_HandleTypeDef * hdma);
extern void checkState();
extern void checkSpeed();
extern void stopTimers();

void initMain() {
	//5√ц 60/12
	Env.freq = 2;
	Env.modulationFreq = 500;
	Env.setPinHI = A1_HI_Pin;
	Env.setPinLO = A2_LO_Pin;
	stopTimers();
	updateTImerValues();

	__HAL_TIM_ENABLE_IT(&htimMain, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&htimMain, TIM_IT_CC1);
	__HAL_TIM_ENABLE_IT(&htim6, TIM_IT_UPDATE);

	//HAL_TIM_OC_Start(&htimMain, TIM_CHANNEL_1);
	//HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	//checkState();
}




void loopMain() {
	HAL_Delay(10);

	checkState();
	startTimers();
	//HAL_GPIO_WritePin(GPIOM, SD_Pin, GPIO_PIN_SET);
	while (1) {
		HAL_Delay(100);
		/*int shortCounter = 10;
		while (shortCounter--) {
			//nop
			HAL_Delay(10);
		}*/
		/*
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
		HAL_Delay(100);
		*/
		//checkSpeed();
	}
}

void adcComplete(DMA_HandleTypeDef * hdma){
	checkSpeed();
}

u32 calculatePeriodAndPrescaler(u32 freq){
	if (freq % 2 != 0)
		freq--; //четна€ частота
	u8 prescaler = 1;
	while ((SystemCoreClock / (freq * prescaler)) > MAX_PERIOD) {
		prescaler++;
	}
	int clock = SystemCoreClock / prescaler;
	u32 period = clock / freq;
	//0 - нет предделител€, 1 - пердделитель на 2...
	prescaler--;
	return period | (prescaler<<16);
}

void updateTImerValues() {

	u32 tmp=Env.freq;
	u32 result=calculatePeriodAndPrescaler(tmp);
	u16 period=result&0xFFFF;
	u8 prescaler=result>>16;

	htimMain.Instance->PSC = prescaler;
	htimMain.Instance->ARR = period;
	htimMain.Instance->CCR1 = period - 10;

	tmp=Env.modulationFreq;
	result=calculatePeriodAndPrescaler(tmp);
	period=result&0xFFFF;
	prescaler=result>>16;

	htim6.Instance->PSC = prescaler;
	htim6.Instance->ARR = period;
	htim6.Instance->CCR3 = period /2;

}

void startTimers(){
	HAL_TIM_OC_Start(&htimMain,TIM_CHANNEL_1);
	HAL_TIM_Base_Start(&htimMain);

	//HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_Base_Start(&htim6);
}

void stopTimers(){
	HAL_TIM_Base_Stop(&htimMain);
}

//get enable pin
//direction pin
//enable or disable timer
void checkState() {
/*
	//enable/disable
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)) {
		if (Env.Enabled == FALSE) {
			startTimers();
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
			Env.Enabled = TRUE;
		}
	} else {
		if (Env.Enabled == TRUE) {
			stopTimers();
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
			Env.Enabled = FALSE;
		}
	}

	//direction
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) { //GPIO_PIN_SET
		Env.Direction = CW;
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	} else {
		Env.Direction = CCW;
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	}
	*/
}

 /*шаг включаетс€ на PeriodElapsed - в этот же момент выключаем предидущий шаг
  * включаем следующий шаг на compare match
  * */

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{


}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim==&htimMain){
		//just setup flag to change step
		Env.ChangeStep=TRUE;
		return;
	}

	if (Env.ChangeStep==TRUE)
	{
		//Disable Previous Step
		HAL_GPIO_WritePin(GPIOM, Env.setPinHI, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOM, Env.setPinLO, GPIO_PIN_RESET);
		switch(Env.CurrentStep){
			case StepA:
				Env.CurrentStep=StepB;
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
				Env.setPinHI = B1_HI_Pin;
				Env.setPinLO = B2_LO_Pin;
				break;
			case StepB:
				Env.CurrentStep=StepC;
				Env.setPinHI = C1_HI_Pin;
				Env.setPinLO = C2_LO_Pin;
				break;
			case StepC:
				Env.CurrentStep=StepAA;
				Env.setPinHI = A2_HI_Pin;
				Env.setPinLO = A1_LO_Pin;
				break;
			case StepAA:
				Env.CurrentStep=StepBB;
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
				Env.setPinHI = B2_HI_Pin;
				Env.setPinLO = B1_LO_Pin;
				break;
			case StepBB:
				Env.CurrentStep=StepCC;
				Env.setPinHI = C2_HI_Pin;
				Env.setPinLO = C1_LO_Pin;
				break;
			case StepCC:
				Env.CurrentStep=StepA;
				Env.setPinHI = A1_HI_Pin;
				Env.setPinLO = A2_LO_Pin;
				break;
		}
		HAL_GPIO_WritePin(GPIOM, Env.setPinHI, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOM, Env.setPinLO, GPIO_PIN_SET);
		Env.ChangeStep=FALSE;
	}else{
		if (Env.HighState==TRUE){
			HAL_GPIO_WritePin(GPIOM, Env.setPinHI, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOM, Env.setPinLO, GPIO_PIN_SET);
			Env.HighState=FALSE;
		}else{
			HAL_GPIO_WritePin(GPIOM, Env.setPinHI, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOM, Env.setPinLO, GPIO_PIN_RESET);
			Env.HighState=TRUE;
		}
	}
}

void checkSpeed() {/*
	u16 max = 0x0FFF;
	bool shouldUpdate = FALSE;

	//proportion speed
	u16 current = ADC_Buf[0];
	float k = (float) current * 1.0F / (float) max;
	float d = k - Env.k;
	if (d < 0)
		d *= (-1);
	if (d > 0.01F) {
		shouldUpdate = TRUE;
		Env.k = k;
	}
	if (Env.k==0){
		Env.k=0.1F;
	}

	//general speed for both motors
	current = ADC_Buf[1];
	u16 delta = 350; //from 150Hz to 500Hz
	u32 addValue = current * delta / max;
	u32 freq = 150 + addValue;
	s32 differ = freq - Env.freq;
	if (differ < 0)
		differ *= (-1);

	if (differ > 20) { //20Hz step
		Env.freq = freq;
		shouldUpdate = TRUE;
	}

	if (shouldUpdate == TRUE) {
		Env.freq2 = Env.freq * Env.k;
		updateTImerValues();
	}*/
}

 void HAL_GPIO_EXTI_Callback(u16 pin){
	 checkState();
 }
/*
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





 void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){
 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);

 if (Env.Enabled==TRUE){

 bank+=increment;
 if (bank>=1.0F){
 bank-=1.0F;
 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
 }

 }
 }



 void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);

 if (Env.Enabled==TRUE){
 //next step logic
 if (Env.Direction==CW){
 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

 if (Env.CurrentStep==3){
 Env.CurrentStep=0;
 }else{
 Env.CurrentStep++;
 }
 }else{
 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
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


 }
 else{
 //reset all bits
 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);
 bank=0;
 }
 }
 */





 /*
 void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim==&htimMain){
		//just setup flag to change step
		Env.ChangeStep=TRUE;
		return;
	}

	if (Env.ChangeStep==TRUE)
	{
		//Disable Previous Step
		HAL_GPIO_WritePin(GPIOM, Env.setPinHI, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOM, Env.setPinLO, GPIO_PIN_RESET);
		switch(Env.CurrentStep){
			case StepA:
				Env.CurrentStep=StepB;
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
				Env.setPinHI = B1_HI_Pin;
				Env.setPinLO = B2_LO_Pin;
				break;
			case StepB:
				Env.CurrentStep=StepC;
				Env.setPinHI = C1_HI_Pin;
				Env.setPinLO = C2_LO_Pin;
				break;
			case StepC:
				Env.CurrentStep=StepAA;
				Env.setPinHI = A2_HI_Pin;
				Env.setPinLO = A1_LO_Pin;
				break;
			case StepAA:
				Env.CurrentStep=StepBB;
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
				Env.setPinHI = B2_HI_Pin;
				Env.setPinLO = B1_LO_Pin;
				break;
			case StepBB:
				Env.CurrentStep=StepCC;
				Env.setPinHI = C2_HI_Pin;
				Env.setPinLO = C1_LO_Pin;
				break;
			case StepCC:
				Env.CurrentStep=StepA;
				Env.setPinHI = A1_HI_Pin;
				Env.setPinLO = A2_LO_Pin;
				break;
		}
		HAL_GPIO_WritePin(GPIOM, Env.setPinHI, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOM, Env.setPinLO, GPIO_PIN_SET);
		Env.ChangeStep=FALSE;
	}else{
		if (Env.HighState==TRUE){
			HAL_GPIO_WritePin(GPIOM, Env.setPinHI, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOM, Env.setPinLO, GPIO_PIN_SET);
			Env.HighState=FALSE;
		}else{
			HAL_GPIO_WritePin(GPIOM, Env.setPinHI, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOM, Env.setPinLO, GPIO_PIN_RESET);
			Env.HighState=TRUE;
		}
	}
}

  */




 /*
 void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
 {

 		switch(Env.CurrentStep){
 		case StepA:
 			//Enable next Step (transition start)
 			HAL_GPIO_WritePin(GPIOM, B1_HI_Pin, GPIO_PIN_SET);
 			HAL_GPIO_WritePin(GPIOM, B2_LO_Pin, GPIO_PIN_SET);
 			break;
 		case StepB:
 			HAL_GPIO_WritePin(GPIOM, C1_HI_Pin, GPIO_PIN_SET);
 			HAL_GPIO_WritePin(GPIOM, C2_LO_Pin, GPIO_PIN_SET);
 			break;
 		case StepC:
 			HAL_GPIO_WritePin(GPIOM, A2_HI_Pin, GPIO_PIN_SET);
 			HAL_GPIO_WritePin(GPIOM, A1_LO_Pin, GPIO_PIN_SET);
 			break;
 		case StepAA:
 			HAL_GPIO_WritePin(GPIOM, B2_HI_Pin, GPIO_PIN_SET);
 			HAL_GPIO_WritePin(GPIOM, B1_LO_Pin, GPIO_PIN_SET);
 			break;
 		case StepBB:
 			HAL_GPIO_WritePin(GPIOM, C2_HI_Pin, GPIO_PIN_SET);
 			HAL_GPIO_WritePin(GPIOM, C1_LO_Pin, GPIO_PIN_SET);
 			break;
 		case StepCC:
 			HAL_GPIO_WritePin(GPIOM, A1_HI_Pin, GPIO_PIN_SET);
 			HAL_GPIO_WritePin(GPIOM, A2_LO_Pin, GPIO_PIN_SET);
 			break;
 	}


 }
 void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
 {

 	switch(Env.CurrentStep){
 		case StepA:
 			Env.CurrentStep=StepB;
 			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);

 			//Disable Step A pins
 			HAL_GPIO_WritePin(GPIOM, A1_HI_Pin, GPIO_PIN_RESET);
 			HAL_GPIO_WritePin(GPIOM, A2_LO_Pin, GPIO_PIN_RESET);
 			break;
 		case StepB:
 			Env.CurrentStep=StepC;
 			//Disable Step B pins
 			HAL_GPIO_WritePin(GPIOM, B1_HI_Pin, GPIO_PIN_RESET);
 			HAL_GPIO_WritePin(GPIOM, B2_LO_Pin, GPIO_PIN_RESET);
 			break;
 		case StepC:
 			Env.CurrentStep=StepAA;
 			HAL_GPIO_WritePin(GPIOM, C1_HI_Pin, GPIO_PIN_RESET);
 			HAL_GPIO_WritePin(GPIOM, C2_LO_Pin, GPIO_PIN_RESET);
 			break;
 		case StepAA:
 			Env.CurrentStep=StepBB;
 			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);

 			HAL_GPIO_WritePin(GPIOM, A2_HI_Pin, GPIO_PIN_RESET);
 			HAL_GPIO_WritePin(GPIOM, A1_LO_Pin, GPIO_PIN_RESET);
 			break;
 		case StepBB:
 			Env.CurrentStep=StepCC;
 			HAL_GPIO_WritePin(GPIOM, B2_HI_Pin, GPIO_PIN_RESET);
 			HAL_GPIO_WritePin(GPIOM, B1_LO_Pin, GPIO_PIN_RESET);
 			break;
 		case StepCC:
 			Env.CurrentStep=StepA;
 			HAL_GPIO_WritePin(GPIOM, C2_HI_Pin, GPIO_PIN_RESET);
 			HAL_GPIO_WritePin(GPIOM, C1_LO_Pin, GPIO_PIN_RESET);
 			break;
 	}

 }
 */
