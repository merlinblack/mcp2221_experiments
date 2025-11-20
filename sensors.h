#ifndef SENSOR_H
#define SENSOR_H

#include <stdbool.h>
#include <stdint.h>

#define MEASUREMENT_FAILURE -1
#define MEASUREMENT_SUCCESS 0

#define BH1750_ADDRESS 0x23
#define BH1750_START_ONESHOT_H_MEASUREMENT 0x20

int get_measurement_bh1750(int i2c, float* lux);

#define AGS10_ADDRESS 0x1a
#define AGS10_TVOC_REG 0x00
#define AGS10_VERSION_REG 0x11
#define AGS10_RESISTANCE_REG 0x20

int get_measurement_ags10(int i2c, bool* ready, uint32_t* tvoc);

#endif  // SENSOR_H
