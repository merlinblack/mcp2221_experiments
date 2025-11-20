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
  int i2c = i2c_open(bus, MCP9808_ADDRESS);
  int ret = EXIT_SUCCESS;

  if (i2c < 0) {
    return EXIT_FAILURE;
  }

  float temperature;

  if (get_measurement_mcp9808(i2c, &temperature)) {
    perror("Error reading device MCP9808");
    ret = EXIT_FAILURE;
  }
  else if (cli_opts.json == true) {
    printf("{\"chip\": \"mcp9808\", \"temperature\": %f }\n", temperature);
  }
  else {
    printf("Temperature: %f\n", temperature);
  }

  close(i2c);

  return ret;
}
