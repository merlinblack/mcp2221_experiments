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
  int i2c = i2c_open(bus, AHT10_ADDRESS);

  if (i2c < 0) {
    perror("Could not open i2c bus");
    return EXIT_FAILURE;
  }

  float temperature;
  float humidity;

  if (get_measurement_aht10(i2c, &temperature, &humidity)) {
    perror("Could not read sensor AHT10");
  }
  else if (cli_opts.json == true) {
    printf(
        "{\"chip\": \"aht10\", \"temperature\": %0.2f, \"humidity\": %0.2f }\n",
        temperature, humidity);
  }
  else {
    printf("Temperature: %0.2f\nHumidity: %0.2f\n", temperature, humidity);
  }

  close(i2c);

  return 0;
}
