#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "i2c.h"
#include "love.h"
#include "options.h"
#include "red5.h"
#include "ssd1306.h"

#define LINES 7

void main(int argc, char **argv)
{
	Options *opt = getOptions(argc, argv);

	int i2c = i2c_open(opt->bus, opt->address);

	if (i2c < 0)
	{
		return;
	}

	ssd1306_begin(i2c);

	if (argc > 0)
	{
		if (!strcmp(argv[optind], "nigelalara"))
		{
			ssd1306_clear();
			ssd1306_full_blit(love);
			ssd1306_show();
			return;
		}
		if (!strcmp(argv[optind], "red5"))
		{
			ssd1306_clear();
			ssd1306_full_blit(red5);
			ssd1306_show();
			return;
		}
	}

	ssd1306_clear();

	char buffer[22];
	uint8_t y = 0;
	for (int i = optind; i <= LINES && i < argc; i++)
	{
		strncpy(buffer, argv[i], 21);
		ssd1306_write_xy(0, y, buffer, opt->textSize);
		y += opt->textSize * 8;
		y += opt->lineSpacing;
	}

	ssd1306_show();
}
