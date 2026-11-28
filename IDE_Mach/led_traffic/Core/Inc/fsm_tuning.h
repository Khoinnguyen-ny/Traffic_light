/*
 * fsm_tuning.h
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 */

#ifndef INC_FSM_TUNING_H_
#define INC_FSM_TUNING_H_

typedef enum {
    MODE_NORMAL = 1,
    MODE_RED,
    MODE_AMBER,
    MODE_GREEN,
	MODE_MANUAL
} Mode_t;

extern Mode_t mode;
extern int temp_time;

void fsm_for_input_processing(void);

#endif /* INC_FSM_TUNING_H_ */
