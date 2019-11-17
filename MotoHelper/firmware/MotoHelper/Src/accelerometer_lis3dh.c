#include "common.h"
#include "accelerometer_manager.h"

I2C_HandleTypeDef *i2c;
#define ADDRESS_READ	0b00110011
#define ADDRESS_WRITE	0b00110010
#define BUFFER_SIZE 6

volatile u8 acc_buf[BUFFER_SIZE];
TaskHandle_t gHandle;

#define STATUS_REG		0x27
#define WHO_AM_I		0x0F
#define OUT_X_L			0x28

//ODR3 ODR2 ODR1 ODR0 LPen Zen Yen Xen
//100Hz
#define CTRL_REG1		0x20
#define CTRL_REG1_VALUE 0b01010111




HAL_StatusTypeDef writeDMA(u8 count){
	return HAL_I2C_Master_Transmit_DMA(i2c, ADDRESS_WRITE,
				&acc_buf, count);
}

HAL_StatusTypeDef readDMA(u8 count){
	return HAL_I2C_Master_Receive_DMA(i2c, ADDRESS_READ,
			&acc_buf, count);
}

HAL_StatusTypeDef writeRegister(u8 reg, u8 value){
	acc_buf[0] = reg;
	acc_buf[1] = value;
	HAL_StatusTypeDef ret = writeDMA(2);
	if (ret == HAL_OK) {
		vTaskSuspend(gHandle);
	}else{
		Error_Handler();
	}
	return ret;
}

HAL_StatusTypeDef readRegister(u8 reg, u8 count){
	if (count>1){
		reg |= 0b10000000;
	}
	acc_buf[0] = reg;
	HAL_StatusTypeDef ret = writeDMA(1);

	if (ret == HAL_OK) {
		vTaskSuspend(gHandle);

		ret = readDMA(count);
		if (ret == HAL_OK) {
			vTaskSuspend(gHandle);
		}else{
			Error_Handler();
		}
	}else{
		Error_Handler();
	}
	return ret;
}

void Accelerometer_Config(I2C_HandleTypeDef *hi2c, TaskHandle_t taskHandle) {

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

reconfigure:
	readRegister(WHO_AM_I, 1);

	if (acc_buf[0] == 0x33)
	{
		writeRegister(CTRL_REG1, CTRL_REG1_VALUE);
		osDelay(2);
		AccelState.State=AccelReady;

	}else{
		goto reconfigure;
	}

}

void ACCEL_readData()
{
	AccelState.State=AccelDataRetriving;
	if (HAL_OK == readRegister(OUT_X_L, 6))
	{
		AccelState.State=AccelDataRetrived;
		return;
	}
	AccelState.State=AccelError;
}

void ACCEL_buildStruct(){
	AccelData.ticks=AccelState.ms;
	AccelData.x = getS16(&acc_buf, 0);
	AccelData.y = getS16(&acc_buf, 2);
	AccelData.z = getS16(&acc_buf, 4);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c == i2c) {
		//vTaskResume(gHandle);
		if (xTaskResumeFromISR(gHandle)) {
			vTaskMissedYield();
		}
	}
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c == i2c) {
		//vTaskResume(gHandle);
		if (xTaskResumeFromISR(gHandle)==pdTRUE) {
			vTaskMissedYield();
		}
	}
}

