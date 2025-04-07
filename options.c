#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "options.h"

extern char* optarg;  // Stop vscode wingeing

struct command_line_options cli_opts = {
    false, -1, false, false, false,
};

struct option long_opts[] = {
    {"bus", required_argument, 0, 'b'},
    {"help", no_argument, 0, 'h'},
    {"verbose", no_argument, 0, 'v'},
    {"csv", no_argument, 0, 'c'},
};

void get_options(int argc, char** argv)
{
  int long_index;
  while (true) {
    int opt = getopt_long(argc, argv, "b:hvc", long_opts, &long_index);
    if (opt == -1) {
      break;
    }

    switch (opt) {
      case 'b':
        cli_opts.bus = atoi(optarg);
        break;
      case 'h':
        fprintf(stderr, "Usage: %s [-b bus] [-h] [-v[v]] [-c]\n", argv[0]);
        fprintf(
            stderr,
            "\t--bus\t\tThe i2c bus number to use. E.g. '1'\n"
            "\t--verbose\tOutput more information. Repeat twice for even "
            "more.\n"
            "\t--csv\t\tOutput results in csv format. (Not implemented yet)\n"
            "\t--help\t\tThis help text.\n"
            "\n");
        exit(0);
      case 'v':
        if (cli_opts.verbose == true) {
          cli_opts.very_verbose = true;
        }
        cli_opts.verbose = true;
        break;
      case 'c':
        cli_opts.csv = true;
        break;
      default:
        break;
    }
  }

  cli_opts.initialised = true;

  return;
}
