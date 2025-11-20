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
  int i2c = i2c_open(bus, AGS10_ADDRESS);

  if (i2c < 0) {
    return EXIT_FAILURE;
  }

  uint32_t reading;
  bool status;

  if (get_measurement_ags10(i2c, &status, &reading)) {
    perror("Error reading device AGS10");
  }
  else if (cli_opts.json == true) {
    printf("{\"chip\": \"ags10\", \"tvoc\": %d }\n", reading);
  }
  else {
    if (cli_opts.verbose == true) {
      printf("Status: %s\n", status ? "Wait" : "Ready");
    }
    printf("TVOC Reading: %d ppb\n", reading);
  }

  close(i2c);

  return 0;
}
