#include "common.h"
#include "soft_timer.h"

volatile struct soft_timer_t soft_timers[SOFT_TIMERS];

volatile u32 ts = 0;

u8 addTimer(u32 period, bool unstop, void (*funcPtr)) {
	u8 i = 0;
	for (i = 0; i < SOFT_TIMERS; i++) {
		volatile struct soft_timer_t *timer = &soft_timers[i];
		if (timer->used == FALSE) {
			timer->used = TRUE;
			timer->period = period;
			timer->next = ts + period;
			timer->unstop = unstop;
			timer->funcPtr = funcPtr;
			return i + 1;
		}
	}
	if (i == SOFT_TIMERS) {
		//hard fault
	}
	return i + 1;
}
void removeTimer(u8 timer_number) {
	if (timer_number > 0) {
		volatile struct soft_timer_t *timer = &soft_timers[timer_number - 1];
		timer->used = FALSE;
	}
}

void restartTimer(u8 timer_number) {
	volatile struct soft_timer_t *timer = &soft_timers[timer_number - 1];
	timer->next = ts + timer->period;
	timer->used = TRUE;
}

void hardwareTimerInvoke() {
	ts += SOFT_TIMER_MS_PER_TICK;

	u8 timer_number = 0;
	for (timer_number = 0; timer_number < SOFT_TIMERS; timer_number++) {
		volatile struct soft_timer_t *timer = &soft_timers[timer_number];
		if (timer->used == TRUE) {
			if (ts >= timer->next) {
				//если бесконечный таймер
				if (timer->unstop == TRUE) {
					//меняем следующее время срабатывания
					timer->next = ts + timer->period;
				} else {
					timer->used = FALSE;
				}
				timer->funcPtr();
			}
		}
	}
}

