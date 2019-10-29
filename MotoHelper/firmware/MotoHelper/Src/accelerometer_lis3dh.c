#include "common.h"
#include "stm32f1xx_hal_i2c.h"

I2C_HandleTypeDef *i2c;
#define ADDRESS_READ	0b00110011
#define ADDRESS_WRITE	0b00110010
#define BUFFER_SIZE 6

volatile u8 acc_buf[BUFFER_SIZE];
u8* bufPtr = &acc_buf;


#define STATUS_REG		0x27
#define WHO_AM_I		0x0F
#define OUT_X_L			0x28

//ODR3 ODR2 ODR1 ODR0 LPen Zen Yen Xen
#define CTRL_REG1		0x20
#define CTRL_REG1_VALUE 0b01010111


TaskHandle_t gHandle;

HAL_StatusTypeDef writeDMA(u8 count){
	return HAL_I2C_Master_Transmit_DMA(i2c, ADDRESS_WRITE,
				&acc_buf, count);
}

HAL_StatusTypeDef readDMA(u8 count){
	return HAL_I2C_Master_Receive_DMA(i2c, ADDRESS_READ,
			&acc_buf, count);
}

void writeRegister(u8 reg, u8 value){
	acc_buf[0] = reg;
	acc_buf[1] = value;
	HAL_StatusTypeDef ret = writeDMA(2);
	if (ret == HAL_OK) {
		vTaskSuspend(gHandle);
	}else{
		Error_Handler();
	}
}

u8 readRegister(u8 reg, u8 count){
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
			return acc_buf[0];
		}else{
			Error_Handler();
		}
	}else{
		Error_Handler();
	}
	return 0;
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

	readRegister(WHO_AM_I, 1);

	if (*bufPtr == 0x33)
	{
		writeRegister(CTRL_REG1, CTRL_REG1_VALUE);
		osDelay(2);
		u8 state = readRegister(STATUS_REG, 1);
		osDelay(1);
		u8 xl = readRegister(OUT_X_L, 6);
		u8 xh = acc_buf[1];

		osDelay(2);
	}

}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c == i2c) {
		//vTaskResume(gHandle);
		if (xTaskResumeFromISR(gHandle) == pdTRUE) {
			vTaskMissedYield();
		}
	}
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c == i2c) {
		//vTaskResume(gHandle);
		if (xTaskResumeFromISR(gHandle) == pdTRUE) {
			vTaskMissedYield();
		}
	}
}

/*
 void ACCEL_Task(){



 }
 */
