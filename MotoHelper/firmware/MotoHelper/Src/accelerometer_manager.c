#include "common.h"
#include "accelerometer_lis3dh.h"

//Hz
#define ACCELEROMETER_FREQ	1000

TIM_HandleTypeDef *accelerationTimer;





/*
	Timer 16 everty 10ms invoking
*/
void ACCEL_Init(TIM_HandleTypeDef *htim, I2C_HandleTypeDef *hi2c, TaskHandle_t taskHandle){
	accelerationTimer = htim;

	TimerConf_t result=calculatePeriodAndPrescaler(ACCELEROMETER_FREQ);
	accelerationTimer->Instance->PSC = result.Prescaler;
	accelerationTimer->Instance->ARR = result.Period;


	Accelerometer_Config(hi2c, taskHandle);

	HAL_TIM_Base_Start_IT(accelerationTimer);
}



void ACCEL_Task(){

  for(;;)
  {
	osDelay(1);
  }
}


void ACCEL_PeriodElapsedCallback() {


}
