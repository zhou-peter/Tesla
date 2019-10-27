#include "common.h"
#include "stm32f1xx_hal_i2c.h"



I2C_HandleTypeDef *i2c;
#define ADDRESS 0x18
#define BUFFER_SIZE 8
volatile u8 acc_buf[BUFFER_SIZE];



#define WHO_AM_I		0x0F






void Accelerometer_Config(I2C_HandleTypeDef *hi2c){

	i2c = hi2c;

	//send command who am I
	//HAL_I2C_Mem_Write_DMA(i2c, ADDRESS, 0, I2C_MEMADD_SIZE_8BIT, &acc_buf, 1);
	HAL_I2C_Mem_Read_DMA(i2c, ADDRESS, WHO_AM_I, I2C_MEMADD_SIZE_8BIT, &acc_buf, 3);

	//await result
	osDelay(10);
	osDelay(10);
}




/*
void ACCEL_Task(){



}
*/
