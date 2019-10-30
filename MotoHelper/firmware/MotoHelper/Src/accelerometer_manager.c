#include "common.h"
#include "accelerometer_manager.h"

//Hz
#define ACCELEROMETER_FREQ	100

TIM_HandleTypeDef *accelerationTimer;
TaskHandle_t gaHandle;
volatile AccelState_t 	AccelState;
volatile AccelData_t 	AccelData;


/*
	Timer 16 everty 10ms invoking
*/
void ACCEL_Init(TIM_HandleTypeDef *htim, I2C_HandleTypeDef *hi2c, TaskHandle_t taskHandle){
	accelerationTimer = htim;
	gHandle = taskHandle;

	TimerConf_t result=calculatePeriodAndPrescaler(ACCELEROMETER_FREQ);
	accelerationTimer->Instance->PSC = result.Prescaler;
	accelerationTimer->Instance->ARR = result.Period;


	Accelerometer_Config(hi2c, taskHandle);

	HAL_TIM_Base_Start_IT(accelerationTimer);
}



void ACCEL_Task(){

  for(;;)
  {
	  if (AccelState.State==AccelShouldRequest){
		  ACCEL_readData();
		  if (AccelState.State==AccelDataRetrived)
		  {
			  ACCEL_buildStruct();
			  AccelState.State=AccelDataReady;
			  vTaskSuspend(gHandle);
		  }
	  }
	  osDelay(1);
  }
}

void ACCEL_ToReadyState(){
	switch (AccelState.State)
	{
		case AccelIdle:
		case AccelError:
		case AccelConfig:
			Error_Handler();
			break;
		case AccelDataReady:
			//normal switch
			AccelState.State=AccelReady;
			break;
		default:
			//should be inverstigated
			AccelState.State=AccelReady;
	}
}

void ACCEL_PeriodElapsedCallback() {
	AccelState.ms++;
	if (AccelState.State==AccelReady)
	{
		AccelState.State=AccelShouldRequest;
		if (xTaskResumeFromISR(gHandle) == pdTRUE) {
			vTaskMissedYield();
		}
	}
}
