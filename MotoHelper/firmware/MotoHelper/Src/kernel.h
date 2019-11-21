#include "common.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#ifndef __KERNEL_H
#define __KERNEL_H

extern void KERNEL_Init(QueueHandle_t queue);
extern void KERNEL_Task();


#endif
