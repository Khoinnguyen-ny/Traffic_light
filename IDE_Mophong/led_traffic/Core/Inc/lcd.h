/*
 * lcd.h
 *
 *  Created on: Nov 17, 2026
 *      Author: Admin
 */

// Thu vien dieu khien man hinh LCD hien thi thong so

#ifndef INC_LCD_H_
#define INC_LCD_H_


#include "main.h"
#include <stdint.h>

#define LCD_RS_PORT  GPIOB
#define LCD_RS_PIN   GPIO_PIN_0
#define LCD_EN_PORT  GPIOB
#define LCD_EN_PIN   GPIO_PIN_1
#define LCD_D4_PORT  GPIOB
#define LCD_D4_PIN   GPIO_PIN_2
#define LCD_D5_PORT  GPIOB
#define LCD_D5_PIN   GPIO_PIN_3
#define LCD_D6_PORT  GPIOB
#define LCD_D6_PIN   GPIO_PIN_4
#define LCD_D7_PORT  GPIOB
#define LCD_D7_PIN   GPIO_PIN_5

void lcd_init(void);
void lcd_cmd(uint8_t c);
void lcd_data(uint8_t d);
void lcd_str(const char *s);
void lcd_goto(uint8_t row, uint8_t col);
void lcd_clear(void);
void lcd_put_number(uint8_t row, uint8_t col, int num, uint8_t width);


#endif /* INC_LCD_H_ */
