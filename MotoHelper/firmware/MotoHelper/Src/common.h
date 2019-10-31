#ifndef __COMMON_H
#define __COMMON_H
#define USE_FULL_ASSERT

#include <stdio.h>

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"

typedef enum {FALSE = 0, TRUE = !FALSE} bool;
#define u32 uint32_t
#define s32 int32_t
#define u16 uint16_t
#define s16 int16_t
#define u8 uint8_t
#define s8 int8_t



typedef struct
{
bool F1:8;
bool F2:8;
bool F3:8;
bool F4:8;
bool F5:8;
bool F6:8;
bool F10:8;
bool LedLight:8;

u8 tmp1:8;
u16	PeriodF1:16;
u16	PeriodF10:16;
} State_t;




typedef struct
{
	u16 Prescaler:16;
	u16 Period:16;
}TimerConf_t;



extern volatile State_t State;

extern TimerConf_t calculatePeriodAndPrescaler(u32 freq);

extern s16 getS16(volatile u8* buf, u8 offset);
#endif
