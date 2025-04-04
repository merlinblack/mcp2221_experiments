#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "options.h"

extern char* optarg;  // Stop vscode wingeing

struct command_line_options cli_opts = {
    false, -1, false, false, false,
};

void get_options(int argc, char** argv)
{
  while (true) {
    int opt = getopt(argc, argv, "b:hvc");
    if (opt == -1) {
      break;
    }

    switch (opt) {
      case 'b':
        cli_opts.bus = atoi(optarg);
        break;
      case 'h':
        fprintf(stderr, "Usage: %s [-b bus] [-h] [-v[v]] [-c]\n", argv[0]);
        exit(0);
      case 'v':
        if (cli_opts.verbose == true) {
          cli_opts.veryverbose = true;
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