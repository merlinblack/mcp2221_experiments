#ifndef __SSD1306_H
#define __SSD1306_H

#include <stdbool.h>
#include <stdint.h>

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

#define SSD1306_I2C_ADDRESS 0x3c

void ssd1306_begin(int i2c);
bool ssd1306_init();
void ssd1306_clear();
bool ssd1306_show();
bool ssd1306_cmd(uint8_t c);
bool ssd1306_power(bool power);
bool ssd1306_invert(bool invert);
bool ssd1306_flip(bool flip);
bool ssd1306_dim(bool dimmed);
void ssd1306_setpixel(uint8_t x, uint8_t y);
void ssd1306_resetpixel(uint8_t x, uint8_t y);
void ssd1306_full_blit(uint8_t* buffer);
void ssd1306_filled_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void ssd1306_writechar(uint8_t x, uint8_t y, char c, uint8_t size);
void ssd1306_write(char* string, uint8_t size);
void ssd1306_write_xy(uint8_t x, uint8_t y, char* string, uint8_t size);

#endif  // __SSD1306_H
