#ifndef __OPTIONS_H
#define __OPTIONS_H

struct Options
{
	unsigned int bus;
	unsigned int address;
	unsigned int lineSpacing;
    unsigned int textSize;
};
typedef struct Options Options;

Options* getOptions(int argc, char **argv);

#endif
