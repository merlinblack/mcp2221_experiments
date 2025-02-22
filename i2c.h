#ifndef __I2C_H
#define __I2C_H

/**
 * Get file handle to i2c bus.
 * Pass negative for address to skip setting address.
*/

int i2c_open(int bus, int address);

/**
 * Select device address, if i2c_open has not already.
 * Can be called more that once on a file handle to change device.
*/
int i2c_select(int file, int address);

#endif // __I2C_H