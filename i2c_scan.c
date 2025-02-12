#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

#include "i2c.h"


int main(int argc, char **argv)
{
    int bus = 0;

    int i2c = i2c_open( bus, -1 );

    if (i2c<0) {
        return 0;
    }

    printf("Write test i2c scan...\n");
    printf("      0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    printf("                            ");

    uint8_t data[1] = { 0x00 };
    
    for (int address = 0x08; address < 0x78; address++) {

        if (i2c_select(i2c, address) == -1) {
            perror("Well this never happens? ");
            break;
        }

        if (address % 0x10 == 0) {
            printf("\n%02x: ", address);
        }

        if (write(i2c, data, 1) == 1) {
            // Found a device.
            printf(" %02X", address);
        }
        else {
            printf(" --");
        }
    }
    
    printf("\n");

    close(i2c);

    return 0;
}

