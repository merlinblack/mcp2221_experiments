#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "getbus.h"
#include "i2c.h"
#include "options.h"

#define AGS10_ADDRESS 0x1a
#define TVOC_REG 0x00
#define VERSION_REG 0x11
#define RESISTANCE_REG 0x20

int main(int argc, char** argv)
{
  int bus = get_bus(argc, argv);

  int i2c = i2c_open(bus, AGS10_ADDRESS);

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

  uint32_t reading = (data[1] << 16) | (data[2] << 8) | data[3];

  if (cli_opts.json == true) {
    printf("{\"chip\": \"ags10\", \"tvoc\": %d }\n", reading);

  } else {
    if (cli_opts.verbose == true) {
      printf("Status: %s\n", data[0] & 0x1 ? "Wait" : "Ready");
    }
    printf("TVOC Reading: %d ppb\n", reading);
  }

cleanup:
  close(i2c);

  return 0;
}
