#include <unistd.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <fcntl.h>

int i2c_open(int bus)
{
    int file;
    char filename[20];

    snprintf(filename, 19, "/dev/i2c-%d", bus);

    file = open(filename, O_RDWR);
    if (file < 0) {
        perror("Error opening i2c bus");
    }

    return file;
}

int i2c_select(int file, int address)
{
    if (ioctl(file, I2C_SLAVE, address) > 0) {
        return -1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    int bus = 0;

    int i2c = i2c_open( bus );

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

