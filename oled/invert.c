#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "i2c.h"
#include "options.h"
#include "ssd1306.h"

int main(int argc, char** argv)
{
  Options* opt = getOptions(argc, argv);

  int i2c = i2c_open(opt->bus, opt->address);
  if (i2c < 0) {
    return EXIT_FAILURE;
  }

  ssd1306_begin(i2c);

  if (argc > optind) {
    if (strcmp(argv[optind], "on") == 0) {
      ssd1306_invert(true);
    } else if (strcmp(argv[optind], "blink") == 0) {
      int count = 1;
      char* errptr;

      if (argc > optind + 1) {
        count = strtoul(argv[optind + 1], &errptr, 0);
      }

      for (int i = 0; i < count; i++) {
        ssd1306_invert(true);
        usleep(200000);
        ssd1306_invert(false);
        usleep(200000);
      }
    } else {
      ssd1306_invert(false);
    }
  } else {
    ssd1306_invert(false);
  }

  return EXIT_SUCCESS;
}
