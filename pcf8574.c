#include <unistd.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <fcntl.h>

/**
 * PCF8574 is designed to sink current rather than source it.
 *
 * It uses "Quasi-bidirectional I/Os". See the data sheet for
 * more information.
 * 
 * This means LEDs should be tied to Vcc via a resistor, and 
 * then to ground via the 8574's port.
 * 
 * There is also no direction register to be set.
 * 
 * This means you write a 1 (HIGH) to turn the LED off and why
 * we send "~bits" below.
 * 
 * 
*/
int i2c_open(int bus, int address)
{
    int file;
    char filename[20];

    snprintf(filename, 19, "/dev/i2c-%d", bus);

    file = open(filename, O_RDWR);
    if (file < 0) {
        perror("Error opening i2c bus");
        return file;
    }

    if (ioctl(file, I2C_SLAVE, address) > 0) {
        perror("Error selecting device");
        close(file);
        return -1;
    }

    return file;
}

int main(int argc, char **argv)
{
    int bus = 0;
    // PCF8574 address 0x20-0x27
    int address = 0x27;

    int i2c = i2c_open( bus, address );

    if (i2c<0) {
        return 0;
    }

    uint8_t data[1] = { 0x00 };
    // Check the first write to make sure the device is really there.
    if (write(i2c, data, 1) != 1) {
        perror("I2C Write to device");
        goto cleanup; // Goto? I've been looking at too much linux kernel drivers.
    }

    uint8_t bits;

    // Fast cylon effect.
    for(int i = 0; i < 15; i++) {
        bits = 1;
        for( int bit = 0; bit < 8; bit++) {
            data[0] = ~bits;
            write(i2c, data, 1);
            bits <<= 1;
            usleep(20000);
        }
        
        bits = 0x80;

        for( int bit = 0; bit < 8; bit++) {
            data[0] = ~bits;
            write(i2c, data, 1);
            bits >>= 1;
            usleep(20000);
        }
    }
    
    data[0] = 0xff;

    write(i2c, data, 1);

cleanup:
    close(i2c);

    return 0;
}

