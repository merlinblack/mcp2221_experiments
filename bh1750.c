#include <stdint.h>
#include <unistd.h>

#include "sensors.h"

int get_measurement_bh1750(int i2c, float* lux)
{
  uint8_t measure_cmd[] = {BH1750_START_ONESHOT_H_MEASUREMENT};
  if (write(i2c, measure_cmd, 1) != 1) {
    return MEASUREMENT_FAILURE;
  }

  usleep(180000);

  uint8_t data[2];
  if (read(i2c, data, 2) != 2) {
    return MEASUREMENT_FAILURE;
  }

  // printf("data: %x %x \n", data[0], data[1]);

  *lux = ((((uint32_t)data[0]) << 8) + data[1]) * 1.2f;

  return MEASUREMENT_SUCCESS;
}
