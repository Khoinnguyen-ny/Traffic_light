/*
 * fsm_tuning.c
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 */

#include "fsm_tuning.h"
#include "button.h"
#include "main.h"
#include <stdio.h>
#include "softwaretimer.h"     // setTimer, timer_flag
#include "fsm_automatic.h"
#include "global.h"            // red_duration, yellow_duration, green_duration, status, INIT
#include "traffic_light.h"     // blinkyRed/Amber/Green, clearAllLEDs
#include "lcd.h"               // lcd_goto, lcd_str, lcd_put_number

Mode_t mode = MODE_NORMAL;
int temp_time = 0;

static uint8_t lcd_dirty = 1;   // 1 = lan ve tiep theo phai ve lai toan bo

// Xoa 2 dong LCD
static void lcd_clear_screen(void) {
    lcd_goto(0, 0); lcd_str("                ");
    lcd_goto(1, 0); lcd_str("                ");
}

// Man hinh chinh thoi gian, cung kieu voi display_countdown_lcd():
//   MODE 2:RED
//   TIME:     05 s
// LCD ghi cham -> chi ve lai khi mode hoac gia tri doi.
static void display_tuning_lcd(int mode_number, int value) {
    static int last_mode = -1, last_value = -1;

    if (!lcd_dirty && last_mode == mode_number && last_value == value) return;
    lcd_dirty  = 0;
    last_mode  = mode_number;
    last_value = value;

    const char *name = "      ";
    switch (mode_number) {
        case 2: name = "RED   "; break;
        case 3: name = "YELLOW"; break;
        case 4: name = "GREEN "; break;
        default: break;
    }

    char line[17];
    snprintf(line, sizeof(line), "MODE %d:%s   ", mode_number, name);   // du 16 ky tu
    lcd_goto(0, 0);
    lcd_str(line);

    lcd_goto(1, 0);
    lcd_str("TIME:     ");
    lcd_put_number(1, 10, value, 3);
    lcd_goto(1, 13);
    lcd_str("s ");
}

static void enter_mode(Mode_t m) {
    mode = m;
    switch (m) {
        case MODE_RED:   temp_time = red_duration;    break;
        case MODE_AMBER: temp_time = yellow_duration; break;
        case MODE_GREEN: temp_time = green_duration;  break;
        default: break;
    }
    // tat het den, reset timer nhay (timer 1) va timer tu dong tang/giam (timer 2)
    clearAllLEDs();
    timer_flag[1] = 1;
    timer_flag[2] = 1;

    // doi man hinh LCD: xoa het roi ve lai
    lcd_clear_screen();
    lcd_dirty = 1;
}

static void increase_temp(void) {
    temp_time++;
    if (temp_time > 99) temp_time = 1;
}

static void decrease_temp(void) {
    temp_time--;
    if (temp_time < 1) temp_time = 99;
}

static void handle_modify(int *target) {
    // INC: nhan 1 lan tang 1
    if (is_button_pressed(BTN_INC_IDX)) {
        increase_temp();
    }
    // DEC: nhan 1 lan giam 1
    if (is_button_pressed(BTN_DEC_IDX)) {
        decrease_temp();
    }

    // Giu INC > 1s: tang lien tuc moi 200ms
    if (is_button_press_1s(BTN_INC_IDX)) {
        if (timer_flag[2] == 1) {
            setTimer(2, 20);   // 20 x 10ms = 200ms
            increase_temp();
        }
    }
    // Giu DEC > 1s: giam lien tuc moi 200ms
    else if (is_button_press_1s(BTN_DEC_IDX)) {
        if (timer_flag[2] == 1) {
            setTimer(2, 20);
            decrease_temp();
        }
    }

    // SELECT: luu gia tri
    if (is_button_pressed(BTN_SELECT_IDX)) {
        *target = temp_time;
    }
}

void fsm_for_input_processing(void) {
    switch (mode) {
        case MODE_NORMAL:
            // Nhan MODE: sang Mode 2 (chinh den Do)
            if (is_button_pressed(BTN_MODE_IDX)) {
                enter_mode(MODE_RED);
            }
            // Bo qua SELECT/INC/DEC o Mode 1 (xoa co de khong bi "nhan nham" khi vao mode chinh)
            is_button_pressed(BTN_SELECT_IDX);
            is_button_pressed(BTN_INC_IDX);
            is_button_pressed(BTN_DEC_IDX);
            break;

        case MODE_RED:
            handle_modify(&red_duration);

            if (timer_flag[1] == 1) {
                setTimer(1, 25);          // 25 x 10ms = 250ms -> nhay 2Hz
                blinkyRed();
            }

            display_tuning_lcd(2, temp_time);

            if (is_button_pressed(BTN_MODE_IDX)) {
                enter_mode(MODE_AMBER);
            }
            break;

        case MODE_AMBER:
            handle_modify(&yellow_duration);

            if (timer_flag[1] == 1) {
                setTimer(1, 25);
                blinkyAmber();
            }

            display_tuning_lcd(3, temp_time);

            if (is_button_pressed(BTN_MODE_IDX)) {
                enter_mode(MODE_GREEN);
            }
            break;

        case MODE_GREEN:
            handle_modify(&green_duration);

            if (timer_flag[1] == 1) {
                setTimer(1, 25);
                blinkyGreen();
            }

            display_tuning_lcd(4, temp_time);

            if (is_button_pressed(BTN_MODE_IDX)) {
                enter_mode(MODE_NORMAL);   // enter_mode da xoa LCD va tat den

                // Dua he thong tu dong ve luc ban dau
                status = INIT;
                clearAllLEDs();
            }
            break;

        default:
            break;
    }
}
