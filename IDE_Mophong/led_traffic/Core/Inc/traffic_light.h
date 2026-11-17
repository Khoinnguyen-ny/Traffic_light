/*
 * traffic_light.h
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 */
#ifndef INC_TRAFFIC_LIGHT_H_
#define INC_TRAFFIC_LIGHT_H_

#include "main.h"

void clearAllLEDs(void);

void setRed1_Green2(void);
void setRed1_Amber2(void);
void setGreen1_Red2(void);
void setAmber1_Red2(void);

void blinkyRed(void);
void blinkyAmber(void);
void blinkyGreen(void);

// Bạn thêm nguyên mẫu hàm bật/tắt LED của bạn vào đây
void display_LED_lane1(int color);
void display_LED_lane2(int color);

// Khai báo Task hiển thị
void display_countdown_lcd(void);
void Task_System_Uptime(void);
void Task_LCD_Display(void);


#endif /* INC_TRAFFIC_LIGHT_H_ */
