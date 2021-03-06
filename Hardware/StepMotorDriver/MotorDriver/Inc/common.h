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
	CW=0,
	CCW
}Directions;

//????????? ?????? ???? ????????? ?? 32 ???? ?? ??????
typedef struct
{

	u8		CurrentStep:8;
	u8		tmp:8;
	bool	Enabled:8;
	Directions Direction:8;


	u32 freq:32;
	u32 freq2:32;
	float k;
} Env_t;




extern volatile Env_t Env;

#endif
