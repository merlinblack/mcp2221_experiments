#include <stdint.h>
#include <unistd.h>

#include "sensors.h"

int get_measurement_aht10(int i2c, float* temperature, float* humidity)
{
  uint8_t init_cmd[] = {AHT10_INIT_CMD, AHT10_CALIBRATION_ENABLE, AHT10_NOP};

  if (write(i2c, init_cmd, 3) != 3) {
    return MEASUREMENT_FAILURE;
  }

  uint8_t measure_cmd[] = {AHT10_START_MEASUREMENT_CMD, 0x33, AHT10_NOP};
  if (write(i2c, measure_cmd, 3) != 3) {
    return MEASUREMENT_FAILURE;
  }

  usleep(75000);

  uint8_t data[6];
  if (read(i2c, data, 6) != 6) {
    return MEASUREMENT_FAILURE;
  }

  // printf( "data: %x %x %x %x %x %x\n", data[0], data[1], data[2], data[3],
  // data[4], data[5] );

  uint32_t raw_humidity = data[1] << 12 | data[2] << 4 | data[3] >> 4;
  uint32_t raw_temperature = (data[3] & 0x0F) << 16 | data[4] << 8 | data[5];

  // 20 bits ^ 2 = 1048576
  *humidity = (raw_humidity / 1048576.0) * 100;
  *temperature = (raw_temperature / 1048576.0) * 200 - 50;

  return MEASUREMENT_SUCCESS;
}
