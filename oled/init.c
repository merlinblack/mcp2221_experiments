#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "i2c.h"
#include "options.h"
#include "red5.h"
#include "ssd1306.h"

int main(int argc, char** argv)
{
  Options* opt = getOptions(argc, argv);

  int i2c = i2c_open(opt->bus, opt->address);
  if (i2c > 0) {
    ssd1306_begin(i2c);
    ssd1306_init();
    ssd1306_clear();
    ssd1306_full_blit(red5);
    ssd1306_show();
  }

  return EXIT_SUCCESS;
}
