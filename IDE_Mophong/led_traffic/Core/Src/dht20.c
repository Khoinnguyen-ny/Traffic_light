/*
 * dht20.c
 *
 *  Created on: Nov 26, 2026
 *      Author: Hai Tran
 */

#include "dht20.h"
#include "global.h"
#include "scheduler.h"

#define SOFT_I2C_PORT      GPIOB
#define SOFT_I2C_SCL_PIN   GPIO_PIN_6
#define SOFT_I2C_SDA_PIN   GPIO_PIN_7

// Khởi tạo 2 chân PB6 (SCL) và PB7 (SDA) ở chế độ Open-Drain
static void Soft_I2C_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SCL_PIN | SOFT_I2C_SDA_PIN, GPIO_PIN_SET);
    GPIO_InitStruct.Pin = SOFT_I2C_SCL_PIN | SOFT_I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SOFT_I2C_PORT, &GPIO_InitStruct);
}

static void Soft_I2C_Delay(void) {
    for (volatile int i = 0; i < 15; i++);
}

static void Soft_I2C_Start(void) {
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_SET);
    Soft_I2C_Delay();
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_RESET);
    Soft_I2C_Delay();
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_RESET);
}

static void Soft_I2C_Stop(void) {
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_RESET);
    Soft_I2C_Delay();
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_SET);
    Soft_I2C_Delay();
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_SET);
    Soft_I2C_Delay();
}

// Gửi 1 byte, trả về 0 nếu nhận được ACK từ Slave, trả về 1 nếu NACK
static uint8_t Soft_I2C_WriteByte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SDA_PIN, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        data <<= 1;
        Soft_I2C_Delay();
        HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_SET);
        Soft_I2C_Delay();
        HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_RESET);
    }

    // Thả SDA và tạo xung clock thứ 9 để đọc bit ACK
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_SET);
    Soft_I2C_Delay();
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_SET);
    Soft_I2C_Delay();
    uint8_t ack = (HAL_GPIO_ReadPin(SOFT_I2C_PORT, SOFT_I2C_SDA_PIN) == GPIO_PIN_RESET) ? 0 : 1;
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_RESET);
    return ack;
}

// Đọc 1 byte; ack = 1 để gửi ACK (đọc tiếp), ack = 0 để gửi NACK (kết thúc đọc)
static uint8_t Soft_I2C_ReadByte(uint8_t ack) {
    uint8_t data = 0;
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_SET);

    for (int i = 0; i < 8; i++) {
        data <<= 1;
        Soft_I2C_Delay();
        HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_SET);
        Soft_I2C_Delay();
        if (HAL_GPIO_ReadPin(SOFT_I2C_PORT, SOFT_I2C_SDA_PIN) == GPIO_PIN_SET) {
            data |= 0x01;
        }
        HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_RESET);
    }

    // Xung clock thứ 9: Master phản hồi ACK hoặc NACK
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SDA_PIN, ack ? GPIO_PIN_RESET : GPIO_PIN_SET);
    Soft_I2C_Delay();
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_SET);
    Soft_I2C_Delay();
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_SET);
    return data;
}

static uint8_t Soft_I2C_Transmit(uint8_t dev_addr, uint8_t *pData, uint16_t size) {
    Soft_I2C_Start();
    if (Soft_I2C_WriteByte(dev_addr & 0xFE) != 0) {
        Soft_I2C_Stop();
        return 1; // NACK địa chỉ
    }
    for (uint16_t i = 0; i < size; i++) {
        Soft_I2C_WriteByte(pData[i]);
    }
    Soft_I2C_Stop();
    return 0;
}

static uint8_t Soft_I2C_Receive(uint8_t dev_addr, uint8_t *pData, uint16_t size) {
    Soft_I2C_Start();
    if (Soft_I2C_WriteByte(dev_addr | 0x01) != 0) {
        Soft_I2C_Stop();
        return 1; // NACK địa chỉ
    }
    for (uint16_t i = 0; i < size; i++) {
        uint8_t send_ack = (i < (size - 1)) ? 1 : 0;
        pData[i] = Soft_I2C_ReadByte(send_ack);
    }
    Soft_I2C_Stop();
    return 0;
}

// Hàm nội bộ: Reset thanh ghi trạng thái nếu cảm biến chưa sẵn sàng
static void DHT20_Reset_Register(uint8_t reg) {
    uint8_t cmd[3] = {reg, 0x00, 0x00};
    uint8_t rx[3] = {0};

    Soft_I2C_Transmit(DHT20_ADDRESS, cmd, 3);
    HAL_Delay(5);
    Soft_I2C_Receive(DHT20_ADDRESS, rx, 3);
    HAL_Delay(10);

    uint8_t write_back[3] = {(uint8_t)(0xB0 | reg), rx[1], rx[2]};
    Soft_I2C_Transmit(DHT20_ADDRESS, write_back, 3);
}

// Khởi tạo cảm biến DHT20 (gọi 1 lần trong USER CODE BEGIN 2 của main.c)
void DHT20_Init(void) {
    Soft_I2C_Init();
    HAL_Delay(100);

    uint8_t status = 0;
    uint8_t cmd = 0x71;

    if (Soft_I2C_Transmit(DHT20_ADDRESS, &cmd, 1) == 0) {
        if (Soft_I2C_Receive(DHT20_ADDRESS, &status, 1) == 0) {
            if ((status & 0x18) != 0x18) {
                DHT20_Reset_Register(0x1B);
                DHT20_Reset_Register(0x1C);
                DHT20_Reset_Register(0x1E);
            }
        }
    }
}

// One-shot task: Đọc 7 byte dữ liệu sau 90 ms (9 ticks) và cập nhật vào global.c
static void DHT20_Read_Callback(void) {
    uint8_t data[7] = {0};

    // Vẫn phát xung đọc I2C ra PB6/PB7 để kiểm tra trên I2C Debugger
    if (Soft_I2C_Receive(DHT20_ADDRESS, data, 7) != 0) {
        // Trên Proteus không có chip DHT20 thật (NACK) -> nạp khung 7 byte chuẩn (50.0% RH, 30.0°C) để test giải mã
        data[0] = 0x18;
        data[1] = 0x80; data[2] = 0x00; data[3] = 0x06;
        data[4] = 0x66; data[5] = 0x66; data[6] = 0x00;
    }

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

void DHT20_Trigger_Task(void) {
    uint8_t trigger_cmd[3] = {0xAC, 0x33, 0x00};
    Soft_I2C_Transmit(DHT20_ADDRESS, trigger_cmd, 3);
    // Luôn gọi bước đọc sau 90 ms (9 ticks) kể cả khi mô phỏng trả NACK
    SCH_Add_Task(DHT20_Read_Callback, 9, 0);
}
