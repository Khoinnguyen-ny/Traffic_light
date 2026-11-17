/*
 * lcd.c
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 */

#include "lcd.h"
#include <stdio.h>

// Delay ngan (xap xi) cho cac thao tac thong thuong
static void lcd_delay_us(uint32_t us) {
    volatile uint32_t n = us * 2;
    while (n--) { __NOP(); }
}

static void lcd_write_nibble(uint8_t nibble) {
    HAL_GPIO_WritePin(LCD_D4_PORT, LCD_D4_PIN, (nibble & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D5_PORT, LCD_D5_PIN, (nibble & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D6_PORT, LCD_D6_PIN, (nibble & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D7_PORT, LCD_D7_PIN, (nibble & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    // Xung EN
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    lcd_delay_us(5);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
    lcd_delay_us(50);
}

void lcd_cmd(uint8_t c) {
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);   // RS = 0: lenh
    lcd_write_nibble(c >> 4);
    lcd_write_nibble(c & 0x0F);
}

void lcd_data(uint8_t d) {
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_SET);     // RS = 1: du lieu
    lcd_write_nibble(d >> 4);
    lcd_write_nibble(d & 0x0F);
}

void lcd_init(void) {
    HAL_Delay(50);                 // cho LCD len nguon

    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    lcd_write_nibble(0x03); HAL_Delay(5);
    lcd_write_nibble(0x03); HAL_Delay(1);
    lcd_write_nibble(0x03); HAL_Delay(1);
    lcd_write_nibble(0x02);        // chuyen sang 4-bit
    HAL_Delay(1);

    lcd_cmd(0x28);                 // 4-bit, 2 dong, font 5x8
    lcd_cmd(0x0C);                 // bat hien thi, tat con tro
    lcd_cmd(0x06);                 // tu dong tang dia chi
    lcd_clear();
}

void lcd_clear(void) {
    lcd_cmd(0x01);
    HAL_Delay(2);                  // lenh xoa can > 1.5ms
}

void lcd_goto(uint8_t row, uint8_t col) {
    uint8_t addr = (row == 0 ? 0x00 : 0x40) + col;
    lcd_cmd(0x80 | addr);
}

void lcd_str(const char *s) {
    while (*s) {
        lcd_data((uint8_t)*s++);
    }
}

// In so nguyen, can phai, do rong 'width' (thieu thi them dau cach)
void lcd_put_number(uint8_t row, uint8_t col, int num, uint8_t width) {
    char buf[12];
    snprintf(buf, sizeof(buf), "%*d", width, num);
    lcd_goto(row, col);
    lcd_str(buf);
}
