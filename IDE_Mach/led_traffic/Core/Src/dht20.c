/*
 * dht20.c
 *
 *  Created on: Nov 26, 2026
 *      Author: Hai Tran
 */
#include "dht20.h"
#include "global.h"
#include "scheduler.h"

extern I2C_HandleTypeDef hi2c1;

// Hàm nội bộ: Reset thanh ghi trạng thái nếu cảm biến chưa sẵn sàng (theo datasheet DHT20)
static void DHT20_Reset_Register(uint8_t reg) {
    uint8_t cmd[3] = {reg, 0x00, 0x00};
    uint8_t rx[3] = {0};

    HAL_I2C_Master_Transmit(&hi2c1, DHT20_ADDRESS, cmd, 3, 100);
    HAL_Delay(5);
    HAL_I2C_Master_Receive(&hi2c1, DHT20_ADDRESS, rx, 3, 100);
    HAL_Delay(10);

    uint8_t write_back[3] = {(uint8_t)(0xB0 | reg), rx[1], rx[2]};
    HAL_I2C_Master_Transmit(&hi2c1, DHT20_ADDRESS, write_back, 3, 100);
}

// Khởi tạo cảm biến DHT20 (chỉ gọi 1 lần khi hệ thống khởi động)
void DHT20_Init(void) {
    HAL_Delay(100); // Chờ cảm biến ổn định nguồn sau khi cấp điện
    uint8_t status = 0;
    uint8_t cmd = 0x71;

    HAL_I2C_Master_Transmit(&hi2c1, DHT20_ADDRESS, &cmd, 1, 100);
    HAL_I2C_Master_Receive(&hi2c1, DHT20_ADDRESS, &status, 1, 100);

    if ((status & 0x18) != 0x18) {
        DHT20_Reset_Register(0x1B);
        DHT20_Reset_Register(0x1C);
        DHT20_Reset_Register(0x1E);
    }
}

// One-shot task: Đọc 7 byte dữ liệu sau 90 ms (9 ticks) và cập nhật vào global.c
static void DHT20_Read_Callback(void) {
    uint8_t data[7] = {0};

    if (HAL_I2C_Master_Receive(&hi2c1, DHT20_ADDRESS, data, 7, 100) != HAL_OK) {
        return; // Lỗi truyền nhận I2C, giữ nguyên giá trị cũ
    }

    // Kiểm tra bit 7 (Busy): 0 = đã đo xong, 1 = đang bận
    if ((data[0] & 0x80) == 0x00) {
        uint32_t raw_humi = ((uint32_t)data[1] << 12) |
                            ((uint32_t)data[2] << 4)  |
                            ((uint32_t)(data[3] >> 4));

        uint32_t raw_temp = (((uint32_t)data[3] & 0x0F) << 16) |
                            ((uint32_t)data[4] << 8)           |
                            ((uint32_t)data[5]);

        dht20_humi = ((float)raw_humi / 1048576.0f) * 100.0f;
        dht20_temp = ((float)raw_temp / 1048576.0f) * 200.0f - 50.0f;

        dht20_humi_x10 = (int)(dht20_humi * 10.0f);
        dht20_temp_x10 = (int)(dht20_temp * 10.0f);
    }
}

// Task định kỳ trên Scheduler (mỗi 2000 ms = 200 ticks)
void DHT20_Trigger_Task(void) {
    uint8_t trigger_cmd[3] = {0xAC, 0x33, 0x00};

    if (HAL_I2C_Master_Transmit(&hi2c1, DHT20_ADDRESS, trigger_cmd, 3, 100) == HAL_OK) {
        // Đăng ký one-shot task (Period = 0) chạy sau 9 ticks (90 ms)
        SCH_Add_Task(DHT20_Read_Callback, 9, 0);
    }
}

