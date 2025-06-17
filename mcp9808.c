#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "getbus.h"
#include "i2c.h"
#include "options.h"

int main(int argc, char** argv)
{
  int bus = get_bus(argc, argv);
  int address = 0x18;  // MCP9808 address

  int i2c = i2c_open(bus, address);

  if (i2c < 0) {
    return 0;
  }

  uint8_t data[3];

  // Cofiguration register 0x01
  // 0x0000 continuous conversion mode
  data[0] = 0x01;
  data[1] = 0x00;
  data[2] = 0x00;

  // Check the result of the first write to make sure
  // device is there.
  if (write(i2c, data, 3) != 3) {
    perror("Error writing to device");
    goto cleanup;
  }

  // Resolution register 0x08
  // 0x03 Resolution +0.0625 / C
  data[0] = 0x08;
  data[1] = 0x03;

  write(i2c, data, 2);

  // Read from register 0x5
  data[0] = 0x05;

  write(i2c, data, 1);

  // Read could fail
  if (read(i2c, data, 2) != 2) {
    goto cleanup;
  }

  // Convert data
  //

  uint16_t raw = ((uint16_t)data[0] << 8) + data[1];

  float temp = raw & 0x0FFF;

  temp /= 16.0;

  if (raw & 0x1000) {
    temp -= 256;
  }

  if (cli_opts.json == true) {
    printf("{\"chip\": \"mcp9808\", \"temperature\": %f }\n", temp);
  } else {
    printf("Temperature: %f\n", temp);
  }

cleanup:
  close(i2c);

  return 0;
}
