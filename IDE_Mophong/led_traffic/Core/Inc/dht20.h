/*
 * dht20.h
 *
 *  Created on: Nov 26, 2026
 *      Author: Hai Tran
 */
#ifndef INC_DHT20_H_
#define INC_DHT20_H_

#include "main.h"

// Địa chỉ I2C 7-bit của DHT20 là 0x38, dịch trái 1 bit cho HAL
#define DHT20_ADDRESS (0x38 << 1)

// Khởi tạo cảm biến DHT20 (gọi 1 lần lúc khởi động trước while(1))
void DHT20_Init(void);

// Task định kỳ cho Scheduler (2000 ms = 200 ticks): Kích đo và hẹn giờ đọc sau 90 ms
void DHT20_Trigger_Task(void);

#endif /* INC_DHT20_H_ */
