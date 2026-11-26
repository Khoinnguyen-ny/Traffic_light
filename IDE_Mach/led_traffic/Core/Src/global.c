/*
 * global.c
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 */

#include "global.h"

int status = INIT;
int counter_lane1 = 0;
int counter_lane2 = 0;

int red_duration = 5;
int green_duration = 3;
int yellow_duration = 2;
int temp_duration = 0;

uint32_t system_uptime = 0;

float dht20_temp = 0.0f;
float dht20_humi = 0.0f;
int dht20_temp_x10 = 0;
int dht20_humi_x10 = 0;
