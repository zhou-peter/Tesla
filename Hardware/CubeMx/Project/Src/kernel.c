#include "kernel.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_tim_ex.h"
#include "timers.h"

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


void packet_02_feature_change(u8* body, u16 bodySize){
	/*u8 num = *(body);
	bool enable = getBool(*(body+1));
	setFeatureState(num, enable);*/
}
void packet_04_timer_config(u8* body, u16 bodySize){
	/*stopTimers();
	resetTimerCounters();

Период несущей
Период проломов
Старт пролома
Стоп пролома
Период пачки
Скважность пачки
Отступ пролома в рабочей области пачки
Стоп пролома в рабочей области пачки
Отступ пропуск верхнее плечо
Стоп пропуска верхнего плеча
Отступ пропуск нижнее плечо
Стоп пропуска нижнее плеча

	u8* p=body;
	htim1.Instance->ARR=getU16(p);
	p+=2;
	htim1.Instance->CCR1=getU16(p);
	htim3.Instance->ARR=getU16(p);
	p+=2;
	htim3.Instance->CCR3=getU16(p);
	p+=2;
	htim3.Instance->CCR4=getU16(p);
	p+=2;
	htim2.Instance->ARR=getU16(p);//Период пачки
	htim4.Instance->ARR=getU16(p);
	htim15.Instance->ARR=getU16(p);
	p+=2;
	htim15.Instance->CCR1=getU16(p);//Скважность пачки
	p+=2;
	htim4.Instance->CCR2=getU16(p);
	p+=2;
	htim4.Instance->CCR4=getU16(p);
	p+=2;
	htim2.Instance->CCR3=getU16(p);//Отступ пропуск верхнее плечо
	p+=2;
	htim2.Instance->CCR4=getU16(p);
	p+=2;
	htim2.Instance->CCR1=getU16(p);
	p+=2;
	htim2.Instance->CCR2=getU16(p);

	startTimers();*/
}


volatile SearchEnv_t SearchState;
TimerHandle_t xTimerSearch;
extern void vTimerSearcher(TimerHandle_t xTimer );

void stopSearchSoftTimer(){
	if (State.SearchTimerEnabled==TRUE){
		xTimerStop(xTimerSearch, 100);
	}
	State.SearchTimerEnabled=FALSE;
}
void startSearchSoftTimer(u16 delay){
	if (xTimerSearch==0){
		xTimerSearch=xTimerCreate("Searcher", pdMS_TO_TICKS(delay), pdTRUE, ( void * ) 0,
				vTimerSearcher);
	}else{
		xTimerChangePeriod(xTimerSearch, pdMS_TO_TICKS(delay), 200);
	}
	State.SearchTimerEnabled=TRUE;
}


void vTimerSearcher(TimerHandle_t xTimer )
{
	if (State.SearcherState==Searching){
		u32 current=htim16.Instance->ARR;
		bool continueSearch=FALSE;
		if (current<SearchState.SearchTo &&
				State.SearchDirection==SearchPeriodIncrease){
			current++;
			continueSearch=TRUE;
		}
		if (current>SearchState.SearchTo &&
				State.SearchDirection==SearchPeriodDecrease){
			current--;
			continueSearch=TRUE;
		}

		if (continueSearch==TRUE){
			htim16.Instance->ARR=current;
			htim16.Instance->CCR1=current/2;
		}else{
			HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim16, TIM_CHANNEL_1);
			State.SearcherState=SearchIdle;
			stopSearchSoftTimer();
		}
	}
}


void packet_06_search(u8* body, u16 bodySize){
/*	u8* p=body;
	State.SearcherState=SearchIdle;

	SearchState.SearchFrom=getU16(p);
	p+=2;
	SearchState.SearchTo=getU16(p);
	p+=2;
	u16 delay=getU16(p);
	if (SearchState.SearchFrom<SearchState.SearchTo){
		State.SearchDirection=SearchPeriodIncrease;
	}else{
		State.SearchDirection=SearchPeriodDecrease;
	}



	htim16.Instance->ARR=SearchState.SearchFrom;
	htim16.Instance->CCR1=SearchState.SearchFrom/2;
	HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim16, TIM_CHANNEL_1);

	State.SearcherState=Searching;
	startSearchSoftTimer(delay);
*/
}
void packet_08_search_stop(u8* body, u16 bodySize){
	/*HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim16, TIM_CHANNEL_1);
	State.SearcherState=SearchIdle;
	stopSearchSoftTimer();*/
}

void packet_0A_just_generate(u8* body, u16 bodySize){
/*	u8* p=body;
	u16 period=getU16(p);
	p+=2;
	u16 duty=getU16(p);
	p+=2;
	u8 feature = *p;



	if (State.SearcherState!=Generating){
		stopSearchSoftTimer();
		HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
		HAL_TIMEx_PWMN_Start(&htim16, TIM_CHANNEL_1);
		State.SearcherState=Generating;
	}



	if (feature==FEATURE_CARRIER){
		htim1.Instance->ARR=period;
		htim1.Instance->CCR1=period/2;//duty;
		htim16.Instance->ARR=(period*2)+1;
		htim16.Instance->CCR1=period;//duty;
		resetTimerCounters();
	}else if (feature==FEATURE_PWR){
		//htim16.Instance->ARR=period;
		//htim16.Instance->CCR1=duty;
	}*/
}

#define t1SmokeTo 8000		//1030;
#define t1Overflow 5004		//1010;
#define t1Switch 5000			//1000;
volatile u32 t1Counter=0;
#define t1PeriodHalfWave 42
#define htimc htim3
#define TIMC TIM3

void prepareCycle(){
	t1Counter=0;
	State.ModeState=ModeHalfWave;
	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
}

void KERNEL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance == TIMC &&
			State.ModeState!=ModeIdle){
		t1Counter++;
        if (t1Counter==t1Switch)
        {
			State.ModeState=ModeQuarterWave;
			htim1.Instance->ARR=t1PeriodHalfWave*2;
			htim1.Instance->CCR1=t1PeriodHalfWave;
        }
        else if (t1Counter==t1Overflow)
        {
			State.ModeState=ModeSmoking;
			htim1.Instance->CCR1=t1PeriodHalfWave/2;
			htim1.Instance->ARR=t1PeriodHalfWave;
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
			HAL_TIM_Base_Stop(&htim1);
        }
        else if (t1Counter==t1SmokeTo) {
        	if (State.ModeState!=ModeIdle){
        		prepareCycle();
        	}else{
        		t1Counter=0;
        	}
		}
	}
}

/*
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){

	if (htim->Instance == TIMC){
        if (__HAL_TIM_GET_FLAG(&htimc, TIM_FLAG_CC1) == SET)
        {
            __HAL_TIM_CLEAR_FLAG(&htimc, TIM_FLAG_CC1);
			State.ModeState=ModeQuarterWave;
        }
        if (__HAL_TIM_GET_FLAG(&htimc, TIM_FLAG_CC2) == SET)
        {
            __HAL_TIM_CLEAR_FLAG(&htimc, TIM_FLAG_CC2);
			State.ModeState=ModeSmoking;
			//HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			//HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
        }
	}
}
*/

void startGeneration(){

	htim1.Instance->ARR=t1PeriodHalfWave;
	htim1.Instance->CCR1=t1PeriodHalfWave/2;

	htimc.Instance->ARR=t1PeriodHalfWave;


	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);

	//HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Base_Start_IT(&htimc);

	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_Base_Start(&htimc);
	State.ModeState=ModeHalfWave;
}

void KERNEL_Init(){
	State.ModeState=ModeIdle;

	
	//__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&htimc, TIM_IT_UPDATE);
	//__HAL_TIM_ENABLE_IT(&htimc, TIM_IT_CC1 );
	//__HAL_TIM_ENABLE_IT(&htimc, TIM_IT_CC2 );


}
extern void KERNEL_Task(){
	osDelay(100);

	startGeneration();
	while(1){
		osDelay(100);
		if (t1Counter>t1SmokeTo)
		{
			prepareCycle();
		}
	}
}


