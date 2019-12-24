#include "common.h"

#define SOFT_TIMER_MS_PER_TICK 5
#define SOFT_TIMERS 7

struct soft_timer_t
{
	u32 period;
	u32 next;
	bool unstop;
	bool used;	//таймер используется
	void (*funcPtr)(); //указатель на функцию
};

extern volatile u32 timeStampCounter;
//добавляет таймер и сразу же запускает его
//возвращает номер таймера для добавления (больше ноля)
extern u8 addTimer(u32 period, bool unstop, void (*funcPtr));

//удаляет таймер.
extern void removeTimer(u8 timer_number);

//рестарт таймера с текущего момента
extern void restartTimer(u8 timer_number);

extern void hardwareTimerInvoke();
