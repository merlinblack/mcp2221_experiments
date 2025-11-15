#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "i2c.h"
#include "options.h"
#include "ssd1306.h"

char* ordinal_suffix_of(int i)
{
  int j = i % 10;
  int k = i % 100;
  if (j == 1 && k != 11) {
    return "st";
  }
  if (j == 2 && k != 12) {
    return "nd";
  }
  if (j == 3 && k != 13) {
    return "rd";
  }
  return "th";
}

int main(int argc, char** argv)
{
  time_t rawtime;
  struct tm* timeinfo;
  char buffer[24];
  int i2c;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  Options* opt = getOptions(argc, argv);

  i2c = i2c_open(opt->bus, opt->address);
  if (i2c < 0) {
    return EXIT_FAILURE;
  }

  ssd1306_begin(i2c);

  ssd1306_clear();

  strftime(buffer, 24, "%B %e", timeinfo);
  ssd1306_write_xy(0, 2, buffer, 1);
  ssd1306_write(ordinal_suffix_of(timeinfo->tm_mday), 1);

  strftime(buffer, 24, "%p", timeinfo);
  ssd1306_write_xy(104, 2, buffer, 2);

  strftime(buffer, 24, "%I:%M", timeinfo);
  ssd1306_write_xy(8, 32, buffer, 4);

  ssd1306_show();

  close(i2c);
}
