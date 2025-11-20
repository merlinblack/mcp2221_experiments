#include <stdint.h>
#include <unistd.h>

#include "sensors.h"

int get_measurement_ags10(int i2c, bool* ready, uint32_t* tvoc)
{
  uint8_t data[5] = {0x00};

  // Check the first write to make sure the device is really there.
  data[0] = AGS10_TVOC_REG;
  if (write(i2c, data, 1) != 1) {
    return MEASUREMENT_FAILURE;
  }

  if (read(i2c, data, 5) != 5) {
    return MEASUREMENT_FAILURE;
  }

  *ready = (bool)(data[0] & 0x01);
  *tvoc = (data[1] << 16) | (data[2] << 8) | data[3];

  return MEASUREMENT_SUCCESS;
}
