#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


int main(int argc, char **argv)
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[24];
    bool dots = true;
    char inhibit[PATH_MAX];
    int i2c;

    Options* opt = getOptions(argc, argv);

    printf("ClockD\nRunning with bus %d and address 0x%02X\n", opt->bus, opt->address);

    i2c = i2c_open(opt->bus, opt->address);

    if (i2c < 0)
    {
        return EXIT_FAILURE;
    }

    if (!getenv("HOME")) {
        fprintf(stderr, "What?! No HOME env var set.\n");
        return EXIT_FAILURE;
    }

    ssd1306_begin(i2c);

    inhibit[0] = 0;
    strncat(inhibit, getenv("HOME"), PATH_MAX-10);
    strncat(inhibit, "/noclock", 9);

    printf("Using inhibit file at: %s\n", inhibit);

    while (true) {
        sleep(1);

        if (access( inhibit, F_OK) == 0) {
            continue;
        }

        time ( &rawtime );
        timeinfo = localtime ( &rawtime );

        if (timeinfo->tm_hour > 1 && timeinfo->tm_hour < 7 ) {
            ssd1306_clear();
            ssd1306_show();
            continue;
        }

        ssd1306_clear();
        strftime(buffer, 24, "%B %e", timeinfo);
        ssd1306_write_xy(0, 2, buffer, 1);
        ssd1306_write(ordinal_suffix_of(timeinfo->tm_mday), 1);
        strftime(buffer, 24, "%p", timeinfo);
        ssd1306_write_xy(104, 2, buffer, 2);

        if (dots) {
            strftime(buffer, 24, "%I:%M", timeinfo);
            dots = false;
        }
        else {
            strftime(buffer, 24, "%I %M", timeinfo);
            dots = true;
        }
        ssd1306_write_xy( 8, 32, buffer, 4);

        ssd1306_show();
    }
}
