/*
 * button.h
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"

// So luong nut bam su dung
#define NO_OF_BUTTONS 4

// Quy uoc chi so mang cho tung nut
#define BTN_MODE_IDX    0   // chuyen che do: 1 -> 2 -> 3 -> 4 -> 1
#define BTN_SELECT_IDX  1   // luu gia tri dang chinh
#define BTN_INC_IDX     2   // tang
#define BTN_DEC_IDX     3   // giam

// Thoi gian nhan giu 1 giay (ngat timer = 10ms -> 100 lan = 1s)
#define DURATION_FOR_AUTO_INCREASING 100

// Muc logic nut bam (pull-up)
#define BUTTON_IS_PRESSED  GPIO_PIN_RESET
#define BUTTON_IS_RELEASED GPIO_PIN_SET

// Cac ham doc va kiem tra trang thai nut
void button_reading(void);
int is_button_pressed(unsigned char index);
int is_button_press_1s(unsigned char index);

#endif /* INC_BUTTON_H_ */

