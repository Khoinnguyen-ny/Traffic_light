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

#define MANUAL_STATES 4

static int manual_state = 0;     // 0..3
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
        "1:D  2:X        ",
        "1:D  2:V        ",
        "1:X  2:D        ",
        "1:V  2:D        "
    };
    if (!manual_dirty) return;
    manual_dirty = 0;

    lcd_goto(0, 0); lcd_str("MODE 5: MANUAL  ");
    lcd_goto(1, 0); lcd_str(txt[manual_state]);
}

void fsm_manual_enter(void) {
    manual_state = 0;
    manual_dirty = 1;
    apply_manual_state();
    display_manual_lcd();
}

int fsm_manual_run(void) {
    // SELECT: đổi sang trạng thái đèn kế tiếp
    if (is_button_pressed(BTN_SELECT_IDX)) {
        manual_state = (manual_state + 1) % MANUAL_STATES;
        apply_manual_state();
        manual_dirty = 1;
    }

    // INC/DEC không dùng ở chế độ này: xóa cờ để không bị "nhấn nhầm"
    is_button_pressed(BTN_INC_IDX);
    is_button_pressed(BTN_DEC_IDX);

    display_manual_lcd();

    // MODE: báo cho fsm_tuning biết cần thoát
    if (is_button_pressed(BTN_MODE_IDX)) return 1;
    return 0;
}
