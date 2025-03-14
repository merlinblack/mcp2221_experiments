#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "i2c.h"
#include "options.h"
#include "ssd1306.h"

void main(int argc, char **argv)
{
  Options *opt = getOptions(argc, argv);

  int i2c = i2c_open(opt->bus, opt->address);
  if (i2c<0) {
    return;
  }
  
  ssd1306_begin(i2c);

  if (argc > optind) {
    if (strcmp(argv[optind], "on") == 0) {
      ssd1306_dim(true);
    }
    else {
      ssd1306_dim(false);
    }
  }
  else {
    ssd1306_dim(false);
  }
}
