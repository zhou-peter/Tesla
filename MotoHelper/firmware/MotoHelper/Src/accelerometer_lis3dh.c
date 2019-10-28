#include "common.h"
#include "stm32f1xx_hal_i2c.h"



I2C_HandleTypeDef *i2c;
#define ADDRESS_READ	0b00110011
#define ADDRESS_WRITE	0b00110010
#define BUFFER_SIZE 8

volatile u8 acc_buf[BUFFER_SIZE];



#define WHO_AM_I		0x0F
TaskHandle_t gHandle;





void Accelerometer_Config(I2C_HandleTypeDef *hi2c, TaskHandle_t taskHandle){

	i2c = hi2c;
	gHandle = taskHandle;

/*
	acc_buf[0] = 0x0F; //Who am I
	HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(i2c,ADDRESS_WRITE, &acc_buf, 1, 5000);

	if (ret == HAL_OK){
		ret = HAL_I2C_Master_Receive(i2c, ADDRESS_READ, &acc_buf, 1, 5000);
		if (ret==HAL_OK){
			acc_buf[3]=acc_buf[0];
		}
	}

*/

	acc_buf[0] = 0x0F; //Who am I
	HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit_DMA(i2c,ADDRESS_WRITE,&acc_buf,1);
	if (ret==HAL_OK){
		vTaskSuspend(gHandle);

		ret = HAL_I2C_Master_Receive_DMA(i2c, ADDRESS_READ, &acc_buf, 1);
		if (ret==HAL_OK){
			vTaskSuspend(gHandle);
			acc_buf[3]=acc_buf[0];
		}
	}



	//HAL_I2C_MasterRxCpltCallback()
//HAL_I2C_MasterTxCpltCallback()

	/*HAL_I2C_Master_Seq_Receive_DMA()


	//send command who am I
	if (HAL_I2C_Master_Receive_DMA(i2c, ADDRESS, &acc_buf, 3)==HAL_OK){
		//await result
		osDelay(10);
		osDelay(10);
	}else{

	}
*/
}


void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if (hi2c==i2c){
		//vTaskResume(gHandle);
		if (xTaskResumeFromISR(gHandle) == pdTRUE){
			vTaskMissedYield();
		}
	}
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c){
	if (hi2c==i2c){
		//vTaskResume(gHandle);
		if (xTaskResumeFromISR(gHandle) == pdTRUE){
			vTaskMissedYield();
		}
	}
}

/*
void ACCEL_Task(){



}
*/
