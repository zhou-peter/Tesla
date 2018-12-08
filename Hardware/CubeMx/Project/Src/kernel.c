#include "kernel.h"
#include "stm32f1xx_hal_tim.h"

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



TIM_HandleTypeDef* getTimer(u8 num){
	switch(num){
		case 1:
			//основной
			return &htim1;
		case 2:
			//пачки
			return &htim15;
		case 3:
			//проломы х2
			return &htim2;
		case 4:
			//пропуски
			return &htim3;
	}
	//hard fault exception
	TIM_HandleTypeDef n;
	return &n;
}


void setTimer(u8 num, bool state){
	TIM_HandleTypeDef* t = getTimer(num);
	if (state==TRUE){
		HAL_TIM_Base_Start(t);
		if (num==1){
			HAL_TIM_PWM_Start(t, TIM_CHANNEL_1);
		}
	}else{
		HAL_TIM_Base_Stop(t);
		if (num==1){
			HAL_TIM_PWM_Stop(t, TIM_CHANNEL_1);
		}
	}

	if (num==1){
		State.TimerF1=state;
	}else if (num==2){
		State.TimerF2=state;
	}else if (num==3){
		State.TimerF3=state;
	}else if (num==4){
		State.TimerF4=state;
	}

}

bool getTimerState(num){
	if (num==1){
		return State.TimerF1;
	}else if (num==2){
		return State.TimerF2;
	}else if (num==3){
		return State.TimerF3;
	}else if (num==4){
		return State.TimerF4;
	}
	return FALSE;
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

	TIM_HandleTypeDef* t = getTimer(num);
	bool timerState=getTimerState(num);
	//если таймер в рабочем состояние, останавливаем его
	if (timerState==TRUE){
		HAL_TIM_Base_Stop(t);
	}


	t->Init.Prescaler=prescaler;
	t->Init.Period=period;
	HAL_TIM_Base_Init(t);

	if (num==1){
		TIM_OC_InitTypeDef sConfigOC;
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = duty;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
		sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

		if (HAL_TIM_PWM_ConfigChannel(t, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
	}


	//если таймер был в рабочем состояние,
	//опять запускаем его
	if (timerState==TRUE){
		HAL_TIM_Base_Start(t);
	}
}
