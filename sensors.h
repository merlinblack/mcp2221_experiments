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

#define MCP9808_ADDRESS 0x18

int get_measurement_mcp9808(int i2c, float* temperature);

#define AHT10_ADDRESS 0x38
#define AHT10_INIT_CMD 0xE1
#define AHT10_START_MEASUREMENT_CMD 0xAC
#define AHT10_CALIBRATION_ENABLE 0x08
#define AHT10_NOP 0x00

int get_measurement_aht10(int i2c, float* temperature, float* humidity);

int get_measurement_bmp280(int i2c, float* temperature, float* pressure);

#endif  // SENSOR_H
