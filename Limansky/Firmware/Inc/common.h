#ifndef __COMMON_H
#define __COMMON_H

//#include "stm32f10x.h"
//#include <CoOs.h>
#include <stdio.h>




typedef enum {FALSE = 0, TRUE = !FALSE} bool;
#define u32 uint32_t
#define s32 int32_t
#define u16 uint16_t
#define s16 int16_t
#define u8 uint8_t
#define s8 int8_t

typedef enum
{
	StepA,
	StepA_B,
	StepB,
	StepB_C,
	StepC,
	StepC_MA,
	StepMA,
	StepMA_MB,
	StepMB,
	StepMB_MC,
	StepMC,
	StepMC_A
}Steps;


typedef enum
{
	CW=0,
	CCW
}Directions;

//структура должна быть выравнена по 32 бита по модулю
typedef struct
{
	Steps	CurrentStep:8;
	u8		tmp:8;
	bool	Enabled:8;
	Directions Direction:8;

	u32 freq:32;
} Env_t;




extern volatile Env_t Env;

#endif
