#include <stdarg.h>
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
  int i2c = i2c_open(bus, -1);

  if (i2c < 0) {
    return EXIT_FAILURE;
  }

  float temperature;
  float pressure;

  if (get_measurement_bmp280(i2c, &temperature, &pressure)) {
    perror("Could not get measurement from BMP280");
  }
  else if (cli_opts.json == true) {
    printf(
        "{\"chip\": \"bmp280\", \"temperature\": %0.2f, \"pressure\": %0.2f "
        "}\n",
        temperature, pressure / 100.0);
  }
  else {
    printf("Temperature: %0.4f deg C\nPressure: %0.2f hPa\n", temperature,
           pressure / 100.0);
  }

  close(i2c);

  return EXIT_SUCCESS;
}
