#include "common.h"
#include "accelerometer_manager.h"



TaskHandle_t gaHandle;
volatile AccelState_t 	AccelState;
AccelData_t 	AccelData;


/*
	Timer 16 everty 10ms invoking
*/
void ACCEL_Init(I2C_HandleTypeDef *hi2c, TaskHandle_t taskHandle){

	gHandle = taskHandle;


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
			  //await another thread
			  ACCEL_Wait();
		  }else{
			  //second attemp in a 1 ms
			  osDelay(1);
		  }
	  }
	  else if (AccelState.State==AccelError){
		  Accelerometer_ReConfig();
	  }
	  else{
		  ACCEL_Wait();
	  }
  }
}



void ACCEL_PeriodElapsedCallback() {
	AccelState.ms+=10;
	if (AccelState.State==AccelDataRetrived)
	{
		AccelState.State=AccelShouldRequest;
		ACCEL_NotifyTaskFromISR();
	}
}
