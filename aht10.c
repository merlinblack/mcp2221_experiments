#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "getbus.h"
#include "i2c.h"
#include "options.h"

#define AHT10_ADDRESS 0x38
#define AHT10_INIT_CMD 0xE1
#define AHT10_START_MEASUREMENT_CMD 0xAC
#define AHT10_CALIBRATION_ENABLE 0x08
#define AHT10_NOP 0x00

int main(int argc, char** argv)
{
  int bus = get_bus(argc, argv);

  int i2c = i2c_open(bus, AHT10_ADDRESS);

  if (i2c < 0) {
    return 0;
  }

  uint8_t init_cmd[] = {AHT10_INIT_CMD, AHT10_CALIBRATION_ENABLE, AHT10_NOP};

  if (write(i2c, init_cmd, 3) != 3) {
    perror("Could not initialise AHt10");
    goto cleanup;
  }

  uint8_t measure_cmd[] = {AHT10_START_MEASUREMENT_CMD, 0x33, AHT10_NOP};
  if (write(i2c, measure_cmd, 3) != 3) {
    perror("Could not send measurement command");
    goto cleanup;
  }

  usleep(75000);

  uint8_t data[6];
  if (read(i2c, data, 6) != 6) {
    perror("Could not read data");
    goto cleanup;
  }

  // printf( "data: %x %x %x %x %x %x\n", data[0], data[1], data[2], data[3],
  // data[4], data[5] );

  uint32_t raw_humidity = data[1] << 12 | data[2] << 4 | data[3] >> 4;
  uint32_t raw_temperature = (data[3] & 0x0F) << 16 | data[4] << 8 | data[5];

  // 20 bits ^ 2 = 1048576
  float humidity = (raw_humidity / 1048576.0) * 100;
  float temperature = (raw_temperature / 1048576.0) * 200 - 50;

  if (cli_opts.json == true) {
    printf("{\"chip\": \"aht10\", \"humidity\": %f, \"temperature\": %f }\n",
           humidity, temperature);
  } else {
    printf("Humidity: %f\nTemperature: %f", humidity, temperature);
  }

cleanup:
  close(i2c);

  return 0;
}
