/*
 * fsm_automatic.c
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 */

#include "fsm_automatic.h"
#include "global.h"
#include "traffic_light.h"

void fsm_automatic_run(void) {
    switch (status) {
		case INIT:
			status = AUTO_RED_GREEN;
			counter_lane1 = red_duration;
			counter_lane2 = green_duration;
			display_LED_lane1(RED);
			display_LED_lane2(GREEN);
			break;

        case AUTO_RED_GREEN:
            counter_lane1--;
            counter_lane2--;
            if (counter_lane2 <= 0) {
                status = AUTO_RED_YELLOW;
                counter_lane2 = yellow_duration;
                display_LED_lane1(RED);
                display_LED_lane2(YELLOW);
            }
            break;

        case AUTO_RED_YELLOW:
            counter_lane1--;
            counter_lane2--;
            if (counter_lane2 <= 0) {
                status = AUTO_GREEN_RED;
                counter_lane1 = green_duration;
                counter_lane2 = red_duration;
                display_LED_lane1(GREEN);
                display_LED_lane2(RED);
            }
            break;

        case AUTO_GREEN_RED:
            counter_lane1--;
            counter_lane2--;
            if (counter_lane1 <= 0) {
                status = AUTO_YELLOW_RED;
                counter_lane1 = yellow_duration;
                display_LED_lane1(YELLOW);
                display_LED_lane2(RED);
            }
            break;

        case AUTO_YELLOW_RED:
            counter_lane1--;
            counter_lane2--;
            if (counter_lane1 <= 0) {
                status = AUTO_RED_GREEN;
                counter_lane1 = red_duration;
                counter_lane2 = green_duration;
                display_LED_lane1(RED);
                display_LED_lane2(GREEN);
            }
            break;

        default:
            break;
    }
}

