#ifndef __OPTIONS_H
#define __OPTIONS_H

struct Options {
  unsigned int bus;
  unsigned int address;
  unsigned int lineSpacing;
  unsigned int textSize;
  unsigned int skip;
  char caption[23];
};
typedef struct Options Options;

Options* getOptions(int argc, char** argv);

#endif
