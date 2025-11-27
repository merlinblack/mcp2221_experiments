#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "getbus.h"
#include "i2c.h"
#include "options.h"
#include "sensors.h"

int main(int argc, char** argv)
{
  int bus = get_bus(argc, argv);
  int i2c = i2c_open(bus, BH1750_ADDRESS);
  int ret = EXIT_SUCCESS;

  if (i2c < 0) {
    perror("Could not open i2c bus");
    return EXIT_FAILURE;
  }

  float lux = 0;

  if (get_measurement_bh1750(i2c, &lux)) {
    perror("Could not get measurment from bh1750");
    ret = EXIT_FAILURE;
  }
  else if (cli_opts.json == true) {
    printf("{\"chip\": \"bh1750\", \"lux\": %0.2f }\n", lux);
  }
  else {
    printf("Lux: %0.2f\n", lux);
  }

  close(i2c);

  return ret;
}
