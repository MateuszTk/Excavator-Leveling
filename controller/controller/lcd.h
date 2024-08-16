#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>
#include <avr/delay.h>

// (- - rs en d7 d6 d5 d4)
#define LCD_DDR DDRC
#define LCD_PORT PORTC
#define LCD_RS PC5
#define LCD_EN PC4
#define LCD_D7 PC3
#define LCD_D6 PC2
#define LCD_D5 PC1
#define LCD_D4 PC0

#define LCD_MODE_CMD 0
#define LCD_MODE_DATA 1

void lcd_init(){
    LCD_DDR = 0xFF;
    LCD_PORT = 0x00;
    
    _delay_ms(15);
    
    lcd_send(0x03, LCD_MODE_CMD);
    _delay_ms(5);
    lcd_send(0x03, LCD_MODE_CMD);
    _delay_us(100);
    lcd_send(0x03, LCD_MODE_CMD);
    _delay_ms(5);
    lcd_send(0x02, LCD_MODE_CMD);
    _delay_ms(1);
    
    // 4-bit mode
    lcd_send(0x28, LCD_MODE_CMD);
    _delay_ms(1);
    // Display on, cursor off, blink off
    lcd_send(0x0C, LCD_MODE_CMD);
    _delay_ms(1);
    // Entry mode
    lcd_send(0x06, LCD_MODE_CMD);
    _delay_ms(1);

    // Clear display
    lcd_clear();
}

void lcd_send(uint8_t data, uint8_t mode){
    LCD_PORT = ((data & 0xF0) >> 4) | (mode << LCD_RS) | (1 << LCD_EN);
    _delay_us(1);
    LCD_PORT &= ~(1 << LCD_EN);
    _delay_us(1);
    
    LCD_PORT = (data & 0x0F) | (mode << LCD_RS) | (1 << LCD_EN);
    _delay_us(1);
    LCD_PORT &= ~(1 << LCD_EN);
    _delay_us(100);
}

void lcd_print(const char* str){
    while (*str){
        lcd_send(*str++, LCD_MODE_DATA);
    }
}

void lcd_print_int(int num, int base){
    char buff[16];
    itoa(num, buff, base);
    lcd_print(buff);
}

void lcd_print_float(float num){
    char buff[16];
    if (num < 0){
        lcd_print("-");
        num = -num;
    }
    itoa((int)num, buff, 10);
    lcd_print(buff);
    lcd_print(".");
    float fraction = num - (int)num;
    sprintf(buff, "%03d", (int)(fraction * 1000.0f));
    lcd_print(buff);
}

void lcd_clear(){
    lcd_send(0x01, LCD_MODE_CMD);
    _delay_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col){
    uint8_t addr = 0x80;
    if (row == 1){
        addr = 0xC0;
    }
    addr += col;
    lcd_send(addr, LCD_MODE_CMD);
}

#endif /* LCD_H_ */