/*
 * fsm_tuning.c
 */

#include "fsm_tuning.h"
#include "fsm_manual.h"
#include "button.h"
#include <stdio.h>
#include "softwaretimer.h"
#include "global.h"
#include "traffic_light.h"
#include "lcd.h"
#include "dht20.h"

Mode_t mode = MODE_NORMAL;
int temp_time = 0;

static uint8_t lcd_dirty = 1;

static void lcd_clear_screen(void) {
    lcd_goto(0, 0); lcd_str("                ");
    lcd_goto(1, 0); lcd_str("                ");
}

static void display_tuning_lcd(int mode_number, int value) {
    static int last_mode = -1, last_value = -1;
    static float last_temp = -999.0f, last_humi = -999.0f;

    if (!lcd_dirty && last_mode == mode_number && last_value == value
        && last_temp == dht20_temp && last_humi == dht20_humi) {
        return;
    }

    lcd_dirty  = 0;
    last_mode  = mode_number;
    last_value = value;
    last_temp  = dht20_temp;
    last_humi  = dht20_humi;

    const char *name = "      ";
    switch (mode_number) {
        case 2: name = "RED   "; break;
        case 3: name = "YEL   "; break;
        case 4: name = "GREEN "; break;
        default: break;
    }

    char line1[17];
    char line2[17];

    snprintf(line1, sizeof(line1), "MODE %d:%s%02ds", mode_number, name, value);
    snprintf(line2, sizeof(line2), "T:%4.1fC  H:%4.1f%%", dht20_temp, dht20_humi);

    lcd_goto(0, 0);
    lcd_str(line1);
    lcd_goto(1, 0);
    lcd_str(line2);
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
    timer_flag[1] = 1;
    timer_flag[2] = 1;

    lcd_clear_screen();
    lcd_dirty = 1;

    is_button_pressed(BTN_SELECT_IDX);
    is_button_pressed(BTN_INC_IDX);
    is_button_pressed(BTN_DEC_IDX);

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

static void handle_modify(Mode_t current_mode) {
    if (is_button_pressed(BTN_INC_IDX)) increase_temp();
    if (is_button_pressed(BTN_DEC_IDX)) decrease_temp();

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

        if (is_button_pressed(BTN_SELECT_IDX)) {
            if (current_mode == MODE_RED) {
                if (temp_time <= yellow_duration)
                    temp_time = yellow_duration + 1;
                red_duration   = temp_time;
                green_duration = red_duration - yellow_duration;
            }
            else if (current_mode == MODE_AMBER) {
                if (temp_time < 1) temp_time = 1;
                if (temp_time >= red_duration)
                    temp_time = red_duration - 1;
                yellow_duration = temp_time;
                green_duration  = red_duration - yellow_duration;
            }
            else if (current_mode == MODE_GREEN) {
                if (temp_time < 1) temp_time = 1;
                if (temp_time + yellow_duration > 99)
                    temp_time = 99 - yellow_duration;
                green_duration = temp_time;
                red_duration   = green_duration + yellow_duration;
            }
            lcd_dirty = 1;
        }
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
            handle_modify(MODE_RED);
            if (timer_flag[1] == 1) { setTimer(1, 250); blinkyRed(); }
            display_tuning_lcd(2, temp_time);
            if (is_button_pressed(BTN_MODE_IDX)) enter_mode(MODE_AMBER);
            break;

        case MODE_AMBER:
            handle_modify(MODE_AMBER);
            if (timer_flag[1] == 1) { setTimer(1, 250); blinkyAmber(); }
            display_tuning_lcd(3, temp_time);
            if (is_button_pressed(BTN_MODE_IDX)) enter_mode(MODE_GREEN);
            break;

        case MODE_GREEN:
            handle_modify(MODE_GREEN);
            if (timer_flag[1] == 1) { setTimer(1, 250); blinkyGreen(); }
            display_tuning_lcd(4, temp_time);
            if (is_button_pressed(BTN_MODE_IDX)) enter_mode(MODE_MANUAL);
            break;

        case MODE_MANUAL:
            if (fsm_manual_run()) {
                enter_mode(MODE_NORMAL);
                status = INIT;
                clearAllLEDs();
            }
            break;

        default:
            break;
    }
}
