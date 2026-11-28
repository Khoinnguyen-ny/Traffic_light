/*
 * traffic_light.c
 *
 *  Created on: Nov 17, 2026
 *      Author: Admin
 */

#include "lcd.h"
#include <stdio.h>

#include "dht20.h"
#include "traffic_light.h"
#include "global.h"

#define CODE_OFF     0u    // 00
#define CODE_YELLOW  1u    // 01
#define CODE_GREEN   2u    // 10
#define CODE_RED     3u    // 11

static uint8_t blink_on = 0;

static uint8_t color_to_code(int color) {
    switch (color) {
        case RED:    return CODE_RED;
        case GREEN:  return CODE_GREEN;
        case YELLOW: return CODE_YELLOW;
        default:     return CODE_OFF;
    }
}

static void lane1_write(uint8_t code) {
    HAL_GPIO_WritePin(TL1_A_GPIO_Port, TL1_A_Pin, (code >> 1) & 1u);
    HAL_GPIO_WritePin(TL1_B_GPIO_Port, TL1_B_Pin, code & 1u);
}

static void lane2_write(uint8_t code) {
    HAL_GPIO_WritePin(TL2_A_GPIO_Port, TL2_A_Pin, (code >> 1) & 1u);
    HAL_GPIO_WritePin(TL2_B_GPIO_Port, TL2_B_Pin, code & 1u);
}

void display_LED_lane1(int color) { lane1_write(color_to_code(color)); }
void display_LED_lane2(int color) { lane2_write(color_to_code(color)); }

void clearAllLEDs(void) {
    blink_on = 0;
    lane1_write(CODE_OFF);
    lane2_write(CODE_OFF);
}

void setRed1_Green2(void)  { display_LED_lane1(RED);    display_LED_lane2(GREEN);  }
void setRed1_Amber2(void)  { display_LED_lane1(RED);    display_LED_lane2(YELLOW); }
void setGreen1_Red2(void)  { display_LED_lane1(GREEN);  display_LED_lane2(RED);    }
void setAmber1_Red2(void)  { display_LED_lane1(YELLOW); display_LED_lane2(RED);    }

static void blink_both(uint8_t code) {
    blink_on = !blink_on;
    lane1_write(blink_on ? code : CODE_OFF);
    lane2_write(blink_on ? code : CODE_OFF);
}

void blinkyRed(void)   { blink_both(CODE_RED);    }
void blinkyAmber(void) { blink_both(CODE_YELLOW); }
void blinkyGreen(void) { blink_both(CODE_GREEN);  }

void Task_System_Uptime(void) {
	system_uptime++;
}

void Task_LCD_Display(void) {
	char line1[17];
	char line2[17];

	switch (status) {
		case INIT:
			sprintf(line1, "TRAFFIC SYSTEM  ");
			sprintf(line2, "INITIALIZING... ");
			break;

		case AUTO_RED_GREEN:
			sprintf(line1, "1:D-%02ds  2:X-%02ds", counter_lane1, counter_lane2);
			sprintf(line2, "SCH:OK  UP:%04lus", system_uptime);
			break;

		case AUTO_RED_YELLOW:
			sprintf(line1, "1:D-%02ds  2:V-%02ds", counter_lane1, counter_lane2);
			sprintf(line2, "SCH:OK  UP:%04lus", system_uptime);
			break;

		case AUTO_GREEN_RED:
			sprintf(line1, "1:X-%02ds  2:D-%02ds", counter_lane1, counter_lane2);
			sprintf(line2, "SCH:OK  UP:%04lus", system_uptime);
			break;

		case AUTO_YELLOW_RED:
			sprintf(line1, "1:V-%02ds  2:D-%02ds", counter_lane1, counter_lane2);
			sprintf(line2, "SCH:OK  UP:%04lus", system_uptime);
			break;

		case MODE_MODIFY_RED:
			sprintf(line1, "MODE: CHINH DO  ");
			sprintf(line2, "THOI GIAN: %02ds  ", temp_duration);
			break;

		case MODE_MODIFY_YELLOW:
			sprintf(line1, "MODE: CHINH VANG   %02ds");
			sprintf(line2, "THOI GIAN:   ", temp_duration);
			break;

		case MODE_MODIFY_GREEN:
			sprintf(line1, "MODE: CHINH XANH");
			sprintf(line2, "THOI GIAN: %02ds  ", temp_duration);
			break;

		default:
			sprintf(line1, "SYSTEM ERROR    ");
			sprintf(line2, "STATUS: %02d      ", status);
			break;
	}

	lcd_goto(0, 0);
		lcd_str(line1);
		lcd_goto(1, 0);
		lcd_str(line2);
}

void display_countdown_lcd(void) {
	char line1[17];
	char line2[17];

	switch (status) {
		case INIT:
			sprintf(line1, "AUTO STARTING...");
			sprintf(line2, "T:%4.1fC  H:%4.1f%%", dht20_temp, dht20_humi);
			break;

		case AUTO_RED_GREEN:
			sprintf(line1, "AUTO 1:R%02d 2:G%02d", counter_lane1, counter_lane2);
			sprintf(line2, "T:%4.1fC  H:%4.1f%%", dht20_temp, dht20_humi);
			break;

		case AUTO_RED_YELLOW:
			sprintf(line1, "AUTO 1:R%02d 2:Y%02d", counter_lane1, counter_lane2);
			sprintf(line2, "T:%4.1fC  H:%4.1f%%", dht20_temp, dht20_humi);
			break;

		case AUTO_GREEN_RED:
			sprintf(line1, "AUTO 1:G%02d 2:R%02d", counter_lane1, counter_lane2);
			sprintf(line2, "T:%4.1fC  H:%4.1f%%", dht20_temp, dht20_humi);
			break;

		case AUTO_YELLOW_RED:
			sprintf(line1, "AUTO 1:Y%02d 2:R%02d", counter_lane1, counter_lane2);
			sprintf(line2, "T:%4.1fC  H:%4.1f%%", dht20_temp, dht20_humi);
			break;

		default:
			break;
	}

	lcd_goto(0, 0);
	lcd_str(line1);
	lcd_goto(1, 0);
	lcd_str(line2);
}
