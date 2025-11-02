#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "getbus.h"
#include "i2c.h"
#include "options.h"

#define BH1750_ADDRESS 0x23
#define BH1750_START_ONESHOT_H_MEASUREMENT 0x20

int main(int argc, char** argv)
{
  int bus = get_bus(argc, argv);

  int i2c = i2c_open(bus, BH1750_ADDRESS);

  if (i2c < 0) {
    return 0;
  }

  uint8_t measure_cmd[] = {BH1750_START_ONESHOT_H_MEASUREMENT};
  if (write(i2c, measure_cmd, 1) != 1) {
    perror("Could not send measurement command");
    goto cleanup;
  }

  usleep(180000);

  uint8_t data[2];
  if (read(i2c, data, 2) != 2) {
    perror("Could not read data");
    goto cleanup;
  }

  //printf("data: %x %x \n", data[0], data[1]);

  float lux = ((((uint32_t)data[0]) << 8) + data[1]) * 1.2f;

  if (cli_opts.json == true) {
    printf("{\"chip\": \"bh1750\", \"lux\": %f }\n", lux);
  } else {
    printf("Lux: %f\n", lux);
  }

cleanup:
  close(i2c);

  return 0;
}
