/*
 * global.h
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "main.h"

// Trạng thái khởi tạo chung
#define INIT                0

// Các trạng thái FSM Tự Động (1 -> 4)
#define AUTO_RED_GREEN      1
#define AUTO_RED_YELLOW     2
#define AUTO_GREEN_RED      3
#define AUTO_YELLOW_RED     4

// Các trạng thái Chỉnh Thời Gian / Manual (11 -> 13)
#define MODE_MODIFY_RED     11
#define MODE_MODIFY_YELLOW  12
#define MODE_MODIFY_GREEN   13

#define RED    0
#define GREEN  1
#define YELLOW 2

extern int status;
extern int counter_lane1;
extern int counter_lane2;
extern int red_duration;
extern int yellow_duration;
extern int green_duration;
extern int temp_duration;

extern uint32_t system_uptime;


#endif /* INC_GLOBAL_H_ */
