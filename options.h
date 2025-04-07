#ifndef __OPTIONS_H
#define __OPTIONS_H

#include <stdbool.h>

struct command_line_options {
  bool initialised;
  int bus;
  bool verbose;
  bool very_verbose;
  bool csv;
};

extern struct command_line_options cli_opts;

void get_options(int argc, char** argv);

#endif  // __OPTIONS_H
