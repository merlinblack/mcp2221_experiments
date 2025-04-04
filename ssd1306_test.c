#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "getbus.h"
#include "i2c.h"
#include "red5.h"
#include "ssd1306.h"

extern int optind;  // from getopt

int main(int argc, char** argv)
{
  int bus = get_bus(argc, argv);
  int address = 0x3c;

  int i2c = i2c_open(bus, address);

  if (i2c < 0) {
    return 0;
  }

  ssd1306_begin(i2c);

  bool clear = false;
  bool init = false;
  bool invert = false;
  bool flip = false;
  bool blit_red5 = false;
  bool bigtext = false;
  bool dim = false;

  while (optind < argc) {
    if (strcmp(argv[optind], "init") == 0) {
      init = true;
    }

    if (strcmp(argv[optind], "clear") == 0) {
      clear = true;
    }

    if (strcmp(argv[optind], "invert") == 0) {
      invert = true;
    }

    if (strcmp(argv[optind], "flip") == 0) {
      flip = true;
    }

    if (strcmp(argv[optind], "red5") == 0) {
      blit_red5 = true;
    }

    if (strcmp(argv[optind], "bigtext") == 0) {
      bigtext = true;
    }

    if (strcmp(argv[optind], "dim") == 0) {
      dim = true;
    }

    optind++;
  }

  if (init) {
    if (ssd1306_init()) {
      perror("ssd1306_init()");
      goto cleanup;
    }
  }

  ssd1306_flip(flip);
  ssd1306_invert(invert);
  ssd1306_dim(dim);

  if (clear == false) {
    if (blit_red5) {
      ssd1306_full_blit(red5);
    } else if (bigtext) {
      for (uint16_t c = 0; c < 252; c++) {
        ssd1306_clear();
        ssd1306_writechar(0, 16, c, 5);
        ssd1306_writechar(30, 16, c + 1, 5);
        ssd1306_writechar(60, 16, c + 2, 5);
        ssd1306_writechar(90, 16, c + 3, 5);
        ssd1306_show(i2c);
        usleep(2000);
      }
      ssd1306_clear();
    } else {
      // Draw some horizontal lines etc
      uint8_t y;
      for (uint8_t x = 0; x < SSD1306_WIDTH; x++) {
        y = (uint8_t)(sin(x / 7.0f) * 8 + 8);

        ssd1306_setpixel(x, y);
        ssd1306_setpixel(x, 16);

        if (x % 4 == 0) {
          ssd1306_setpixel(x, 15);
          ssd1306_resetpixel(x, 16);
        }
      }

      ssd1306_write_xy(0, 24, "Nigel was here.", 1);
      ssd1306_write_xy(0, 32, "ABCDEFGHIJKLMNOPQRSTU", 1);
      ssd1306_write_xy(0, 40, "VWXYZ 0987654321", 1);
      ssd1306_write_xy(0, 48, "abcdefghijklmnopqrstu", 1);
      ssd1306_write_xy(0, 56, "vwxyz !@#$%^&*()", 1);
    }
  }

  if (ssd1306_show(i2c)) {
    perror("ssd1306_show()");
  }

cleanup:
  close(i2c);

  return 0;
}
