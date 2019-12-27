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

#define CTRL_REG1		0x20
#define CTRL_REG1_VALUE 0b10010111




HAL_StatusTypeDef writeDMA(u8 count){
	return HAL_I2C_Master_Transmit_DMA(i2c, ADDRESS_WRITE,
				&acc_buf, count);
}

HAL_StatusTypeDef readDMA(u8 count){
	return HAL_I2C_Master_Receive_DMA(i2c, ADDRESS_READ,
			&acc_buf, count);
}

void ACCEL_Wait() {
	const TickType_t xBlockTime = pdMS_TO_TICKS(30);
	u32 ticks1 = xTaskGetTickCount();
	ulTaskNotifyTake(pdFALSE, xBlockTime);
	u32 ticks2 = xTaskGetTickCount();
	ticks2-=ticks1;

}

void ACCEL_Error(){
	AccelState.State=AccelError;
}

HAL_StatusTypeDef writeRegister(u8 reg, u8 value){
	acc_buf[0] = reg;
	acc_buf[1] = value;
	HAL_StatusTypeDef ret = writeDMA(2);
	if (ret == HAL_OK) {
		ACCEL_Wait();
	}else{
		ACCEL_Error();
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
		ACCEL_Wait();

		ret = readDMA(count);
		ACCEL_Wait();
		if (ret != HAL_OK) {
			ACCEL_Error();
		}
	}else{
		ACCEL_Error();
	}
	return ret;
}






void Accelerometer_ReConfig(){
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
	Accelerometer_ReConfig();
}

void ACCEL_readData()
{
	AccelState.State=AccelDataRetriving;
	if (HAL_OK == readRegister(OUT_X_L, 6))
	{
		AccelState.State=AccelDataRetrived;
		return;
	}
	ACCEL_Error();
}

void ACCEL_buildStruct(){
	AccelData.x = getS16(&acc_buf, 0)>>6;
	AccelData.y = getS16(&acc_buf, 2)>>6;
	AccelData.z = getS16(&acc_buf, 4)>>6;
}

void ACCEL_NotifyTaskFromISR() {
	BaseType_t false = pdFALSE;
	vTaskNotifyGiveFromISR(gHandle, &false);
	taskYIELD();
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c == i2c) {
		ACCEL_NotifyTaskFromISR();
	}
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c == i2c) {
		ACCEL_NotifyTaskFromISR();
	}
}

