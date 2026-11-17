/*
 * button.c
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 */

#include "button.h"
#include "main.h"

// --- DOC NUT NHAN ---
static GPIO_TypeDef* BUTTON_Ports[NO_OF_BUTTONS] = {
    BTN_MODE_GPIO_Port,
    BTN_SELECT_GPIO_Port,
    BTN_INC_GPIO_Port,
    BTN_DEC_GPIO_Port
};

static uint16_t BUTTON_Pins[NO_OF_BUTTONS] = {
    BTN_MODE_Pin,
    BTN_SELECT_Pin,
    BTN_INC_Pin,
    BTN_DEC_Pin
};

static unsigned char last_button[NO_OF_BUTTONS] = {
    BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED
};
static unsigned char filtered_button[NO_OF_BUTTONS] = {
    BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED
};

static uint8_t  flagForButtonPress1s[NO_OF_BUTTONS]    = {0};
static uint16_t counterForButtonPress1s[NO_OF_BUTTONS] = {0};
static uint8_t  button_flag[NO_OF_BUTTONS]             = {0};

void button_reading(void) {
    for (int i = 0; i < NO_OF_BUTTONS; i++) {
        unsigned char raw_button = HAL_GPIO_ReadPin(BUTTON_Ports[i], BUTTON_Pins[i]);

        // 2 lan doc lien tiep (cach 10ms) giong nhau moi chap nhan (chong doi)
        if (last_button[i] == raw_button) {
            // Canh xuong da loc nhieu: RELEASED -> PRESSED = nhan 1 lan
            if (filtered_button[i] == BUTTON_IS_RELEASED && raw_button == BUTTON_IS_PRESSED) {
                button_flag[i] = 1;
            }
            filtered_button[i] = raw_button;
        }
        last_button[i] = raw_button;

        // Nhan giu tren 1 giay
        if (filtered_button[i] == BUTTON_IS_PRESSED) {
            if (counterForButtonPress1s[i] < DURATION_FOR_AUTO_INCREASING) {
                counterForButtonPress1s[i]++;
            } else {
                flagForButtonPress1s[i] = 1;
            }
        } else {
            counterForButtonPress1s[i] = 0;
            flagForButtonPress1s[i]    = 0;
        }
    }
}

int is_button_pressed(unsigned char index) {
    if (index >= NO_OF_BUTTONS) return 0;

    if (button_flag[index] == 1) {
        button_flag[index] = 0;   // xoa co
        return 1;
    }
    return 0;
}

int is_button_press_1s(unsigned char index) {
    if (index >= NO_OF_BUTTONS) return 0;
    return flagForButtonPress1s[index];
}
