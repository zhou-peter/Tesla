#include "common.h"
#include "accelerometer_manager.h"



TaskHandle_t gaHandle;
volatile AccelState_t 	AccelState;
AccelData_t 	AccelData;
QueueHandle_t	accelQueue;

/*
	Timer 16 everty 10ms invoking
*/
void ACCEL_Init(I2C_HandleTypeDef *hi2c, TaskHandle_t taskHandle, QueueHandle_t queue){

	gHandle = taskHandle;
	accelQueue = queue;


	Accelerometer_Config(hi2c, taskHandle);

}



void ACCEL_Task(){

  for(;;)
  {
	  if (AccelState.State==AccelShouldRequest){
		  ACCEL_readData();
		  if (AccelState.State==AccelDataRetrived)
		  {
			  ACCEL_buildStruct();
			  //put new data to the queue;
			  xQueueSend(accelQueue, ( void * ) &AccelData,
									 pdMS_TO_TICKS(50));
			  AccelState.State=AccelDataReady;
			  ACCEL_Wait();
		  }else{
			  osDelay(1);
			  AccelState.State=AccelShouldRequest;
		  }
	  }else{
		  osDelay(1);
	  }
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
	AccelState.ms+=10;
	if (AccelState.State==AccelReady)
	{
		AccelState.State=AccelShouldRequest;
		ACCEL_NotifyTaskFromISR();
	}
}
