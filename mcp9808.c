#include <unistd.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <fcntl.h>

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

int i2c_write(int file, uint8_t* data, int len)
{
    if (write(file, data, len) != len) {
        perror("Error writing");
        return -1;
    }
    return len;
}

int i2c_read(int file, uint8_t* buffer, int len)
{
    if (read(file, buffer, len) != len) {
        perror("Error readiing");
        return -1;
    }
    return len;
}


int main(int argc, char **argv)
{
    int bus = 0;
    // MCP9808 address
    int address = 0x18;

    int i2c = i2c_open( bus, address );

    if (i2c<0) {
        return 0;
    }

    uint8_t data[3];

    // Cofiguration register 0x01
    // 0x0000 continuous conversion mode
    data[0] = 0x01;
    data[1] = 0x00;
    data[2] = 0x00;

    if (i2c_write(i2c, data, 3) < 0) {
        goto cleanup;
    }

    // Resolution register 0x08
    // 0x03 Resolution +0.0625 / C
    data[0] = 0x08;
    data[1] = 0x03;

    if (i2c_write(i2c, data, 2) < 0) {
        goto cleanup;
    }

    // Read from register 0x5
    data[0] = 0x05;

    if (i2c_write(i2c, data, 1) < 0) {
        goto cleanup;
    }

    if (i2c_read(i2c, data, 2) < 0) {
        goto cleanup;
    }

    // Convert data from 13bits MSB first.
    float temp = ((data[0] & 0x1f) << 8) + data[1];

    if (temp > 4095) {
        temp -= 8192;
    }

    // Adjust to set resolution
    temp *= 0.0625;

    printf("Temperature: %f\n", temp);
    
cleanup:
    close(i2c);

    return 0;
}

