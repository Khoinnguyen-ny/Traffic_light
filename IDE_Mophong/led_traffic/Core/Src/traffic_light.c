/*
 * traffic_light.c
 *
 *  Created on: Nov 17, 2026
 *      Author: Admin
 */

#include "traffic_light.h"
#include "global.h"
#include "lcd.h"
#include <stdio.h>


void clearAllLEDs(void) {
    HAL_GPIO_WritePin(RED_1_GPIO_Port, RED_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AMBER_1_GPIO_Port, AMBER_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GREEN_1_GPIO_Port, GREEN_1_Pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(RED_2_GPIO_Port, RED_2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AMBER_2_GPIO_Port, AMBER_2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GREEN_2_GPIO_Port, GREEN_2_Pin, GPIO_PIN_SET);
}

void setRed1_Green2(void) {
    clearAllLEDs();
    HAL_GPIO_WritePin(RED_1_GPIO_Port, RED_1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GREEN_2_GPIO_Port, GREEN_2_Pin, GPIO_PIN_RESET);
}

void setRed1_Amber2(void) {
    clearAllLEDs();
    HAL_GPIO_WritePin(RED_1_GPIO_Port, RED_1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AMBER_2_GPIO_Port, AMBER_2_Pin, GPIO_PIN_RESET);
}

void setGreen1_Red2(void) {
    clearAllLEDs();
    HAL_GPIO_WritePin(GREEN_1_GPIO_Port, GREEN_1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RED_2_GPIO_Port, RED_2_Pin, GPIO_PIN_RESET);
}

void setAmber1_Red2(void) {
    clearAllLEDs();
    HAL_GPIO_WritePin(AMBER_1_GPIO_Port, AMBER_1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RED_2_GPIO_Port, RED_2_Pin, GPIO_PIN_RESET);
}

void blinkyRed(void) {
    HAL_GPIO_WritePin(AMBER_1_GPIO_Port, AMBER_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GREEN_1_GPIO_Port, GREEN_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AMBER_2_GPIO_Port, AMBER_2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GREEN_2_GPIO_Port, GREEN_2_Pin, GPIO_PIN_SET);

    HAL_GPIO_TogglePin(RED_1_GPIO_Port, RED_1_Pin);
    HAL_GPIO_TogglePin(RED_2_GPIO_Port, RED_2_Pin);
}

void blinkyAmber(void) {
    HAL_GPIO_WritePin(RED_1_GPIO_Port, RED_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GREEN_1_GPIO_Port, GREEN_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RED_2_GPIO_Port, RED_2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GREEN_2_GPIO_Port, GREEN_2_Pin, GPIO_PIN_SET);

    HAL_GPIO_TogglePin(AMBER_1_GPIO_Port, AMBER_1_Pin);
    HAL_GPIO_TogglePin(AMBER_2_GPIO_Port, AMBER_2_Pin);
}

void blinkyGreen(void) {
    HAL_GPIO_WritePin(RED_1_GPIO_Port, RED_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AMBER_1_GPIO_Port, AMBER_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RED_2_GPIO_Port, RED_2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AMBER_2_GPIO_Port, AMBER_2_Pin, GPIO_PIN_SET);

    HAL_GPIO_TogglePin(GREEN_1_GPIO_Port, GREEN_1_Pin);
    HAL_GPIO_TogglePin(GREEN_2_GPIO_Port, GREEN_2_Pin);
}

void display_LED_lane1(int color) {
    if (color == RED) {
        HAL_GPIO_WritePin(RED_1_GPIO_Port, RED_1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GREEN_1_GPIO_Port, GREEN_1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(AMBER_1_GPIO_Port, AMBER_1_Pin, GPIO_PIN_SET);
    } else if (color == GREEN) {
        HAL_GPIO_WritePin(RED_1_GPIO_Port, RED_1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GREEN_1_GPIO_Port, GREEN_1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(AMBER_1_GPIO_Port, AMBER_1_Pin, GPIO_PIN_SET);
    } else if (color == YELLOW) {
        HAL_GPIO_WritePin(RED_1_GPIO_Port, RED_1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GREEN_1_GPIO_Port, GREEN_1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(AMBER_1_GPIO_Port, AMBER_1_Pin, GPIO_PIN_RESET);
    }
}

void display_LED_lane2(int color) {
    if (color == RED) {
        HAL_GPIO_WritePin(RED_2_GPIO_Port, RED_2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GREEN_2_GPIO_Port, GREEN_2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(AMBER_2_GPIO_Port, AMBER_2_Pin, GPIO_PIN_SET);
    } else if (color == GREEN) {
        HAL_GPIO_WritePin(RED_2_GPIO_Port, RED_2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GREEN_2_GPIO_Port, GREEN_2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(AMBER_2_GPIO_Port, AMBER_2_Pin, GPIO_PIN_SET);
    } else if (color == YELLOW) {
        HAL_GPIO_WritePin(RED_2_GPIO_Port, RED_2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GREEN_2_GPIO_Port, GREEN_2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(AMBER_2_GPIO_Port, AMBER_2_Pin, GPIO_PIN_RESET);
    }
}

// Bạn tự dán code hàm display_LED_lane1 và display_LED_lane2 của bạn vào đây nhé

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
            sprintf(line1, "MODE: CHINH VANG");
            sprintf(line2, "THOI GIAN: %02ds  ", temp_duration);
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
            sprintf(line1, "MODE: AUTO      ");
            sprintf(line2, "STARTING...     ");
            break;

        case AUTO_RED_GREEN:
            sprintf(line1, "MODE: AUTO      ");
            sprintf(line2, "1:D-%02ds  2:X-%02ds", counter_lane1, counter_lane2);
            break;

        case AUTO_RED_YELLOW:
            sprintf(line1, "MODE: AUTO      ");
            sprintf(line2, "1:D-%02ds  2:V-%02ds", counter_lane1, counter_lane2);
            break;

        case AUTO_GREEN_RED:
            sprintf(line1, "MODE: AUTO      ");
            sprintf(line2, "1:X-%02ds  2:D-%02ds", counter_lane1, counter_lane2);
            break;

        case AUTO_YELLOW_RED:
            sprintf(line1, "MODE: AUTO      ");
            sprintf(line2, "1:V-%02ds  2:D-%02ds", counter_lane1, counter_lane2);
            break;

        default:
            break;
    }

    lcd_goto(0, 0);
            lcd_str(line1);
            lcd_goto(1, 0);
            lcd_str(line2);
}
