/*
 * fsm_manual.h
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 */

#ifndef INC_FSM_MANUAL_H_
#define INC_FSM_MANUAL_H_

void fsm_manual_enter(void);   // gọi 1 lần khi vào chế độ bằng tay
int  fsm_manual_run(void);     // gọi mỗi 10ms; trả về 1 khi người dùng muốn thoát


#endif /* INC_FSM_MANUAL_H_ */
