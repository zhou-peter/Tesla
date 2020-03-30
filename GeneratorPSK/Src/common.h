#ifndef __COMMON_H
#define __COMMON_H
#define USE_FULL_ASSERT

#include <stdio.h>

#include "stm32f1xx_hal.h"
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
	u16 Prescaler:16;
	u16 Period:16;
}TimerConf_t;



extern TimerConf_t calculatePeriodAndPrescaler(u32 freq);
extern s16 getS16(volatile u8* buf, u8 offset);
extern void copy(void* src, void* dst, u16 count);

extern volatile void nop();
extern volatile void deadtime();

#endif
