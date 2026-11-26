/*
 * softwaretimer.h
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 */

#ifndef INC_SOFTWARETIMER_H_
#define INC_SOFTWARETIMER_H_

#define MAX_TIMERS 10

extern int timer_flag[MAX_TIMERS];
#define TIMER_TICK_MS 10

void setTimer(int index, int duration);
void timerRun(void);


#endif /* INC_SOFTWARETIMER_H_ */
