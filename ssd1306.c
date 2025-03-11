#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "i2c.h"
#include "getbus.h"

extern int optind;  // from getopt

#define WIDTH 128
#define HEIGHT 64

#define SET_CONTRAST 0x81
#define SET_ENTIRE_ON 0xA4
#define SET_NORM_INV 0xA6
#define SET_DISP 0xAE
#define SET_MEM_ADDR 0x20
#define SET_COL_ADDR 0x21
#define SET_PAGE_ADDR 0x22
#define SET_DISP_START_LINE 0x40
#define SET_SEG_REMAP 0xA0
#define SET_MUX_RATIO 0xA8
#define SET_IREF_SELECT 0xAD
#define SET_COM_OUT_DIR 0xC0
#define SET_DISP_OFFSET 0xD3
#define SET_COM_PIN_CFG 0xDA
#define SET_DISP_CLK_DIV 0xD5
#define SET_PRECHARGE 0xD9
#define SET_VCOM_DESEL 0xDB
#define SET_CHARGE_PUMP 0x8D

// H * W / 8 + 1 (for SET_DISP_START_LINE command)
#define BUFFER_SIZE HEIGHT * WIDTH / 8 + 1
uint8_t buffer[BUFFER_SIZE] = {0};
uint8_t *framebuffer = &buffer[1];

bool ssd1306_show(int i2c)
{
    buffer[0] = SET_DISP_START_LINE;

    if ((write(i2c, buffer, BUFFER_SIZE)) != BUFFER_SIZE) {
        return true;
    }

    return false;
}

bool ssd1306_cmd(int i2c, uint8_t c)
{
    uint8_t control[2] = { 0x80, c};	// Co = 0, D/C = 0

    if ( write(i2c, control, 2) != 2) {
        return true;
    }

    return false;
}

bool ssd1306_init(int i2c, uint8_t width, uint8_t height)
{
    uint8_t com_pin_cfg = 0x12;

    if (width > 2 * height) {
        com_pin_cfg = 0x02;
    }

    uint8_t commands[] = {
        SET_DISP,               // display off
        SET_MEM_ADDR, 0x00,     // horizontal addressing mode
        SET_DISP_START_LINE,
        SET_SEG_REMAP | 0x01,   // col addr 127 mapped to SEG0
        SET_MUX_RATIO,
        height - 1,
        SET_COM_OUT_DIR | 0x08, // scan from COM[n] to COM0
        SET_DISP_OFFSET, 0x00,
        SET_COM_PIN_CFG, com_pin_cfg,
        // timing and driving scheme
        SET_DISP_CLK_DIV, 0x80,
        SET_PRECHARGE, 0xF1,    // no external vcc
        SET_VCOM_DESEL, 0x30,   // 0.83*Vcc - n.b. specs for ssd1306 for 64x32 screens imply this should be 0x40
                                // display
        SET_CONTRAST, 0xFF,     // maximum!
        SET_ENTIRE_ON,          // output is entire RAM contents
        SET_NORM_INV,           // not inverted
        SET_IREF_SELECT, 0x30,  // enable internal IREF during display on
        SET_CHARGE_PUMP, 0x14,
        // addresses
        SET_COL_ADDR, 0, width - 1,
        SET_PAGE_ADDR, 0, (int)(height/8) - 1,
        SET_DISP | 0x01         // display on
    };

    uint8_t n = sizeof(commands) / sizeof(commands[0]);

    for (uint8_t i = 0; i < n; i++)
    {
        if (ssd1306_cmd(i2c, commands[i])) {
            return true;
        }
    }

    return false;
}

int main(int argc, char **argv)
{
    int bus = get_bus(argc, argv);
    int address = 0x3c;

    int i2c = i2c_open( bus, address );

    if (i2c<0) {
        return 0;
    }

    bool clear = false;
    bool init = false;

    while (optind < argc) {

        if (strcmp(argv[optind], "init") == 0) {
            init = true;
        }

        if (strcmp(argv[optind], "clear") == 0) {
            clear = true;
        }

        optind++;
    }

    if (init) {
        ssd1306_init(i2c, WIDTH, HEIGHT);
    }

    if (clear == false) {

        // Draw some horizontal lines etc
        uint8_t y;
        for (uint8_t x = 0; x < WIDTH; x++) {
            y = (uint8_t)(sin(x / 7.0f) * 8 + 8);

            framebuffer[(y / 8) * WIDTH + x] |= 1 << (y & 7);
            y = 32;
            framebuffer[(y / 8) * WIDTH + x] |= 1 << (y & 7);
            y = 63;
            framebuffer[(y / 8) * WIDTH + x] |= 1 << (y & 7);

            if (x % 4 == 0) {
                y = 31;
                framebuffer[(y / 8) * WIDTH + x] |= 1 << (y & 7);
            }
        }
    }

    if (ssd1306_show(i2c)) {
        perror("ssd1306_show()");
    }

    close(i2c);

    return 0;
}

