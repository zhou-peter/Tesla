#include "kernel.h"


void changeTimerState(u8 num, u8 enabled){
	bool en=FALSE;
	if (enabled>0)
		en=TRUE;

	if (num==1){
		State.TimerF1=en;
	}
}

bool getBool(u8* ptr){
	bool result=FALSE;
	if (*ptr>0){
		result=TRUE;
	}
	return result;
}

u16 getU16(u8* ptr){
	u16 result=0;
	result |= *ptr;
	result |= (*(ptr+1))<<8;
	return result;
}

void setTimer(u8 num, bool state){
	if (num==1){
		State.TimerF1=state;
	}
}

TIM_HandleTypeDef getTimer(u8 num){
	switch(num){
		case 1:
			//основной
			return htim1;
		case 2:
			//пачки
			return htim15;
		case 3:
			//проломы х2
			return htim2;
		case 4:
			//пропуски
			return htim3;
	}
	//hard fault exception
	return 0;
}


void packet_02_timer_enable(u8* body, u16 bodySize){
	u8 num = *(body);
	bool enable = getBool(*(body+1));
	setTimer(num, enable);
}
void packet_04_timer_config(u8* body, u16 bodySize){
	u8 num = *(body);
	u8 prescaler = *(body+1);
	u16 period = getU16(body+2);
	u16 duty = getU16(body+4);

	TIM_HandleTypeDef t = getTimer(num);
	t.Init.Prescaler=prescaler;

}
