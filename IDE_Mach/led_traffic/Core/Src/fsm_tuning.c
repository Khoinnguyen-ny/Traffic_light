/*
 * fsm_tuning.c
 */

#include "fsm_tuning.h"
#include "fsm_manual.h"
#include "button.h"
#include <stdio.h>
#include "softwaretimer.h"     // setTimer, timer_flag
#include "global.h"            // red_duration, yellow_duration, green_duration, status, INIT
#include "traffic_light.h"     // blinkyRed/Amber/Green, clearAllLEDs
#include "lcd.h"               // lcd_goto, lcd_str, lcd_put_number

Mode_t mode = MODE_NORMAL;
int temp_time = 0;

static uint8_t lcd_dirty = 1;   // 1 = lan ve tiep theo phai ve lai toan bo

static void lcd_clear_screen(void) {
    lcd_goto(0, 0); lcd_str("                ");
    lcd_goto(1, 0); lcd_str("                ");
}

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
    snprintf(line, sizeof(line), "MODE %d:%s   ", mode_number, name);
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

    clearAllLEDs();
    timer_flag[1] = 1;      // nhay ngay
    timer_flag[2] = 1;      // cho phep tang/giam khi giu nut ngay

    lcd_clear_screen();
    lcd_dirty = 1;

    // xoa co nut con sot, tranh "nhan nham" o mode moi
    is_button_pressed(BTN_SELECT_IDX);
    is_button_pressed(BTN_INC_IDX);
    is_button_pressed(BTN_DEC_IDX);

    // phai goi SAU lcd_clear_screen, neu khong man hinh manual bi xoa mat
    if (m == MODE_MANUAL) fsm_manual_enter();
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
    if (is_button_pressed(BTN_INC_IDX)) increase_temp();
    if (is_button_pressed(BTN_DEC_IDX)) decrease_temp();

    // Giu INC/DEC > 1s: lap moi 200ms
    if (is_button_press_1s(BTN_INC_IDX)) {
        if (timer_flag[2] == 1) {
            setTimer(2, 200);
            increase_temp();
        }
    } else if (is_button_press_1s(BTN_DEC_IDX)) {
        if (timer_flag[2] == 1) {
            setTimer(2, 200);
            decrease_temp();
        }
    }

    *target = temp_time;                 // tu luu luon
    is_button_pressed(BTN_SELECT_IDX);   // SELECT khong dung o mode chinh
}

void fsm_for_input_processing(void) {
    switch (mode) {
        case MODE_NORMAL:
            if (is_button_pressed(BTN_MODE_IDX)) enter_mode(MODE_RED);
            is_button_pressed(BTN_SELECT_IDX);
            is_button_pressed(BTN_INC_IDX);
            is_button_pressed(BTN_DEC_IDX);
            break;

        case MODE_RED:
            handle_modify(&red_duration);
            if (timer_flag[1] == 1) { setTimer(1, 250); blinkyRed(); }
            display_tuning_lcd(2, temp_time);
            if (is_button_pressed(BTN_MODE_IDX)) enter_mode(MODE_AMBER);
            break;

        case MODE_AMBER:
            handle_modify(&yellow_duration);
            if (timer_flag[1] == 1) { setTimer(1, 250); blinkyAmber(); }
            display_tuning_lcd(3, temp_time);
            if (is_button_pressed(BTN_MODE_IDX)) enter_mode(MODE_GREEN);
            break;

        case MODE_GREEN:
            handle_modify(&green_duration);
            if (timer_flag[1] == 1) { setTimer(1, 250); blinkyGreen(); }
            display_tuning_lcd(4, temp_time);
            if (is_button_pressed(BTN_MODE_IDX)) enter_mode(MODE_MANUAL);   // <-- sang MANUAL
            break;

        case MODE_MANUAL:
            if (fsm_manual_run()) {          // tra ve 1 khi nguoi dung nhan MODE
                enter_mode(MODE_NORMAL);
                status = INIT;               // auto chay lai tu dau
                clearAllLEDs();
            }
            break;

        default:
            break;
    }
}
