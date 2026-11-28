/*
 * fsm_manual.c
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 */

#include "fsm_manual.h"
#include "button.h"
#include "traffic_light.h"
#include "lcd.h"
#include "softwaretimer.h"
#include "dht20.h"

#define MANUAL_STATES 4
#define MANUAL_TIMEOUT_MS   10000
#define MANUAL_TIMEOUT_SEC 10

static int manual_state = 0;        // 0..3
static int manual_countdown = MANUAL_TIMEOUT_SEC;
static uint8_t manual_dirty = 1;

static void apply_manual_state(void) {
    switch (manual_state) {
        case 0: setRed1_Green2(); break;
        case 1: setRed1_Amber2(); break;
        case 2: setGreen1_Red2(); break;
        case 3: setAmber1_Red2(); break;
        default: break;
    }
}

static void display_manual_lcd(void) {
    static const char *txt[MANUAL_STATES] = {
        "1:R 2:G",
        "1:R 2:Y",
        "1:G 2:R",
        "1:Y 2:R"
    };
    static float last_temp = -999.0f, last_humi = -999.0f;

    if (!manual_dirty && last_temp == dht20_temp && last_humi == dht20_humi) {
        return;
    }
    manual_dirty = 0;
    last_temp = dht20_temp;
    last_humi = dht20_humi;

    char line1[17];
    char line2[17];

    snprintf(line1, sizeof(line1), "MAN %s  %02ds", txt[manual_state], manual_countdown);
    snprintf(line2, sizeof(line2), "T:%4.1fC  H:%4.1f%%", dht20_temp, dht20_humi);

    lcd_goto(0, 0);
    lcd_str(line1);
    lcd_goto(1, 0);
    lcd_str(line2);
}

void fsm_manual_enter(void) {
    manual_state = 0;
    manual_countdown = MANUAL_TIMEOUT_SEC;
    manual_dirty = 1;
    apply_manual_state();
    setTimer(3, 1000);
    display_manual_lcd();
}

int fsm_manual_run(void) {
    if (is_button_pressed(BTN_SELECT_IDX)) {
        manual_state = (manual_state + 1) % MANUAL_STATES;
        manual_countdown = MANUAL_TIMEOUT_SEC;
        apply_manual_state();
        setTimer(3, 1000);
        manual_dirty = 1;
    }

    is_button_pressed(BTN_INC_IDX);
    is_button_pressed(BTN_DEC_IDX);

    if (timer_flag[3] == 1) {
        manual_countdown--;
        if (manual_countdown <= 0) {
            timer_flag[3] = 0;
            return 1;
        }
        setTimer(3, 1000);
        manual_dirty = 1;
    }

    display_manual_lcd();

    if (is_button_pressed(BTN_MODE_IDX)) {
        timer_flag[3] = 0;
        return 1;
    }
    return 0;
}
