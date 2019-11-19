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
			  struct AccelData_t *pxMessage = &AccelData;
			  xQueueSend(accelQueue, ( void * ) &pxMessage,
									 pdMS_TO_TICKS(50));
			  AccelState.State=AccelShouldRequest;
			  ACCEL_Wait();
		  }else{
			  osDelay(1);
		  }
	  }
	  else if (AccelState.State==AccelReady){
		  AccelState.State=AccelShouldRequest;
		  ACCEL_Wait();
	  }
	  else if (AccelState.State==AccelError){
		  Accelerometer_ReConfig();
	  }
	  else{
		  osDelay(1);
	  }
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
