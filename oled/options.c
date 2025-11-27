#include "options.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ssd1306.h"

Options* getOptions(int argc, char** argv)
{
  static Options options;
  // Defaults
  options.bus = 1;
  options.address = SSD1306_I2C_ADDRESS;
  options.lineSpacing = 0;
  options.textSize = 1;
  options.skip = 0;
  options.caption[0] = 0;

  int opt;
  char* errptr;
  while ((opt = getopt(argc, argv, "a:b:c:k:s:t:h")) != -1) {
    switch (opt) {
      case 'a':
        options.address = strtoul(optarg, &errptr, 0);
        break;
      case 'b':
        options.bus = strtoul(optarg, &errptr, 0);
        break;
      case 'c':
        strncpy(options.caption, optarg, 21);
        options.caption[22] = 0;
        break;
      case 'k':
        options.skip = strtoul(optarg, &errptr, 0);
        break;
      case 's':
        options.lineSpacing = strtoul(optarg, &errptr, 0);
        break;
      case 't':
        options.textSize = strtoul(optarg, &errptr, 0);
        break;
      case 'h':
        printf(
            "Usage: %s [options] [text|red5|nigelalara] [on|off]\n"
            "-a address\n"
            "-b bus\n"
            "-c caption\n"
            "-k skip\n"
            "-s line spacing\n"
            "-t text size\n",
            argv[0]

        );
        exit(0);
      case '?':
        if (optopt == 'a' || optopt == 'b' || optopt == 'c' || optopt == 'k' ||
            optopt == 's' || optopt == 't')
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint(optopt))
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        exit(1);
      default:
        abort();
    }
  }

  return &options;
}
