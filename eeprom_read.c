#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "getbus.h"
#include "i2c.h"

int main(int argc, char** argv)
{
  int bus = get_bus(argc, argv);
  int device_address = 0x52;  // eeprom address
  ssize_t sz = 0x1000;
  uint8_t result[sz];

  int i2c = i2c_open(bus, device_address);

  if (i2c < 0) {
    return 0;
  }

  // Read AT24C32, the whole 4k
  uint8_t address[2] = {0, 0};

  if (write(i2c, address, 2) != 2) {
    perror("EEPROM Write address");
    goto cleanup;
  }

  if (read(i2c, &result, sz) != sz) {
    perror("EEPROM Read 4k");
    goto cleanup;
  }

  FILE* fp = fopen("dump", "w");
  fwrite(result, sz, 1, fp);
  fclose(fp);

cleanup:
  close(i2c);

  return 0;
}
