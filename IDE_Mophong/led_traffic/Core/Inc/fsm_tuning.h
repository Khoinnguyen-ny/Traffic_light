/*
 * fsm_tuning.h
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 */

#ifndef INC_FSM_TUNING_H_
#define INC_FSM_TUNING_H_


// Cac che do hoat dong cua he thong den giao thong
typedef enum {
    MODE_NORMAL = 1,
    MODE_RED,
    MODE_AMBER,
    MODE_GREEN
} Mode_t;

// Bien toan cuc chia se voi cac file khac
extern Mode_t mode;
extern int temp_time;   // gia tri thoi gian dang tam chinh

// Ham FSM xu ly nut nhan
void fsm_for_input_processing(void);

#endif /* INC_FSM_TUNING_H_ */
