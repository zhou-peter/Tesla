#include "common.h"
#include "kernel.h"
#include "soft_timer.h"
#include "communication_manager.h"
#include "accelerometer_manager.h"

QueueHandle_t	accelQueue;

void KERNEL_Init(QueueHandle_t queue){
	accelQueue = queue;
}

void KERNEL_Task(){

}


