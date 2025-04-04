#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "getbus.h"
#include "i2c.h"

#define AGS10_ADDRESS 0x1a;
#define TVOC_REG 0x00;
#define VERSION_REG 0x11;
#define RESISTANCE_REG 0x20;

int main(int argc, char** argv)
{
  int bus = get_bus(argc, argv);
  int address = 0x1a;

  int i2c = i2c_open(bus, address);

  if (i2c < 0) {
    return 0;
  }

  uint8_t data[5] = {0x00};

  // Check the first write to make sure the device is really there.
  data[0] = TVOC_REG;
  if (write(i2c, data, 1) != 1) {
    perror("I2C Write to device");
    goto cleanup;  // Goto? I've been looking at too much linux kernel drivers.
                   // :-D
  }

  if (read(i2c, data, 5) != 5) {
    perror("Expected to read 5 bytes");
    goto cleanup;
  }

  uint32_t a = data[1];
  uint32_t b = data[2];
  uint32_t c = data[3];
  uint32_t reading = (a << 16) | (b << 8) | c;

  printf("Status: %s\n", data[0] & 0x1 ? "Wait" : "Ready");
  // printf("Bytes: 0x%02x 0x%02x 0x%02x\n", data[1], data[2], data[3]);
  printf("TVOC Reading: %d ppb\n", reading);

cleanup:
  close(i2c);

  return 0;
}
