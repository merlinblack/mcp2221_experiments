#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "ssd1306.h"
#include "font5x8.h"

#define SET_CONTRAST 0x81
#define SET_ENTIRE_ON 0xa4
#define SET_NORM_INV 0xa6
#define SET_DISP 0xae
#define SET_MEM_ADDR 0x20
#define SET_COL_ADDR 0x21
#define SET_PAGE_ADDR 0x22
#define SET_DISP_START_LINE 0x40
#define SET_SEG_REMAP 0xa0
#define SET_MUX_RATIO 0xa8
#define SET_IREF_SELECT 0xad
#define SET_COM_OUT_DIR 0xc0
#define SET_DISP_OFFSET 0xd3
#define SET_COM_PIN_CFG 0xda
#define SET_DISP_CLK_DIV 0xd5
#define SET_PRECHARGE 0xd9
#define SET_VCOM_DESEL 0xdb
#define SET_CHARGE_PUMP 0x8d

// H * W / 8 (pixels per byte) + 1 (for SET_DISP_START_LINE command)
#define BUFFER_SIZE SSD1306_HEIGHT * SSD1306_WIDTH / 8 + 1
uint8_t buffer[BUFFER_SIZE] = {0};
uint8_t *framebuffer = &buffer[1];
uint8_t _write_x = 0;
uint8_t _write_y = 0;
int _i2c;

void ssd1306_begin(int i2c)
{
    _i2c = i2c;
}

bool ssd1306_show()
{
    buffer[0] = 0x40; // Write to RAM

    if ((write(_i2c, buffer, BUFFER_SIZE)) != BUFFER_SIZE)
    {
        return true;
    }

    return false;
}

inline bool ssd1306_cmd(uint8_t c)
{
    uint8_t control[2] = {0x80, c}; // Co = 0, D/C = 0

    if (write(_i2c, control, 2) != 2)
    {
        return true;
    }

    return false;
}

bool ssd1306_init()
{
    uint8_t com_pin_cfg = 0x12;

    if (SSD1306_WIDTH > 2 * SSD1306_HEIGHT)
    {
        com_pin_cfg = 0x02;
    }

    uint8_t commands[] = {
        SET_DISP,           // display off
        SET_MEM_ADDR, 0x00, // horizontal addressing mode
        SET_DISP_START_LINE,
        SET_SEG_REMAP | 0x01, // col addr 127 mapped to SEG0
        SET_MUX_RATIO, SSD1306_HEIGHT - 1,
        SET_COM_OUT_DIR | 0x08, // scan from COM[n] to COM0
        SET_DISP_OFFSET, 0x00,
        SET_COM_PIN_CFG, com_pin_cfg,
        // timing and driving scheme
        SET_DISP_CLK_DIV, 0x80,
        SET_PRECHARGE, 0xf1,   // no external vcc
        SET_VCOM_DESEL, 0x30,  // 0.83*Vcc - n.b. specs for ssd1306 for 64x32 screens imply this should be 0x40
                               // display
        SET_CONTRAST, 0xff,    // maximum!
        SET_ENTIRE_ON,         // output is entire RAM contents
        SET_NORM_INV,          // not inverted
        SET_IREF_SELECT, 0x30, // enable internal IREF during display on
        SET_CHARGE_PUMP, 0x14,
        // addresses
        SET_COL_ADDR, 0, SSD1306_WIDTH - 1,
        SET_PAGE_ADDR, 0, (int)(SSD1306_HEIGHT / 8) - 1,
        SET_DISP | 0x01 // display on
    };

    uint8_t n = sizeof(commands) / sizeof(commands[0]);

    for (uint8_t i = 0; i < n; i++)
    {
        if (ssd1306_cmd(commands[i]))
        {
            return true;
        }
    }

    return false;
}

void ssd1306_clear()
{
    memset(framebuffer, 0, BUFFER_SIZE-1);
}

bool ssd1306_power(bool power)
{
    return ssd1306_cmd(SET_DISP | (power & 1));
}

bool ssd1306_invert(bool invert)
{
    return ssd1306_cmd(SET_NORM_INV | (invert & 1));
}

bool ssd1306_dim(bool dimmed)
{
    uint8_t contrast = 0x7f;

    if (dimmed)
    {
        contrast = 0x01;
    }

    if (ssd1306_cmd(SET_CONTRAST))
    {
        return true;
    }

    return ssd1306_cmd(contrast);
}

bool ssd1306_flip(bool flip)
{
    flip = !flip; // false = normal

    if (ssd1306_cmd(SET_SEG_REMAP | (flip & 1)))
    {
        return true;
    }

    return ssd1306_cmd(SET_COM_OUT_DIR | ((flip & 1) << 3));
}

inline void ssd1306_setpixel(uint8_t x, uint8_t y)
{
    framebuffer[(y / 8) * SSD1306_WIDTH + x] |= 1 << (y & 7);
}

inline void ssd1306_resetpixel(uint8_t x, uint8_t y)
{
    framebuffer[(y / 8) * SSD1306_WIDTH + x] &= ~(1 << (y & 7));
}

void ssd1306_full_blit(uint8_t *buffer)
{
    // framebuffer is one byte shorter than BUFFER_SIZE
    memcpy(framebuffer, buffer, BUFFER_SIZE-1);
}

void ssd1306_filled_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    for (uint8_t row = x; row < x + w; row++)
    {
        for (uint8_t col = y; col < y + h; col++)
        {
            ssd1306_setpixel(row, col);
        }
    }
}

void ssd1306_writechar(uint8_t x, uint8_t y, char c, uint8_t size)
{
    // Check we have room
    if (x + size * 5 > SSD1306_WIDTH)
        return;
    if (y + size * 8 > SSD1306_HEIGHT)
        return;

    for (uint8_t col = 0; col < 5; col++) 
    {
        uint8_t fontdata = font5x8[c * 5 + col];

        if (fontdata)
        {
            if (size == 1)
            {
                for (uint8_t offset = 0; offset < 8; offset++)
                {
                    if (fontdata & 0x1)
                    {
                        ssd1306_setpixel(x, y + offset);
                    }

                    fontdata >>= 1;
                }
            }
            else
            {
                for (uint8_t offset = 0; offset < 8 * size; offset += size)
                {
                    if (fontdata & 0x1)
                    {
                        ssd1306_filled_rect(x, y + offset, size, size);
                    }

                    fontdata >>= 1;
                }
            }
        }

        x += size;
    }
}

void ssd1306_write_xy(uint8_t x, uint8_t y, char *string, uint8_t size)
{
    _write_x = x;
    _write_y = y;
    ssd1306_write(string, size);
}

void ssd1306_write(char *string, uint8_t size)
{
    while (*string)
    {
        if (*string == '\n')
        {
            _write_x = 0;
            _write_y += 8;
            string++;
            continue;
        }
        if (*string == '\r')
        {
            string++;
            continue;
        }

        ssd1306_writechar(_write_x, _write_y, *string++, size);
        _write_x += 6 * size;
    }
}
