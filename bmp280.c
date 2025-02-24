#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>

#include "bmp2.h"
#include "i2c.h"
#include "getbus.h"

BMP2_INTF_RET_TYPE bmp2_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);
BMP2_INTF_RET_TYPE bmp2_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);
void bmp2_delay_us(uint32_t period, void *intf_ptr);

char *get_bmp2_error(int8_t code);

#define NOISY 0

void vlog(char *fmt, ...)
{
    va_list args;

    if (NOISY) {
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
    }
}

int main(int argc, char **argv)
{
    int bus = get_bus(argc, argv);
    int address = BMP2_I2C_ADDR_PRIM;

    int i2c = i2c_open(bus, address);

    if (i2c<0) {
        return -1;
    }

    int8_t ret;

    struct bmp2_dev device;
    device.intf = BMP2_I2C_INTF;
    device.intf_ptr = (void *)&i2c;
    device.read = bmp2_read;
    device.write = bmp2_write;
    device.delay_us = bmp2_delay_us;

    vlog("bmp2_init()\n");
    if (BMP2_OK != (ret = bmp2_init(&device))) {
        vlog("Whoops! %s\n", get_bmp2_error(ret));
        goto cleanup;
    }

    struct bmp2_config config;

    vlog("bmp2_get_config()\n");
    if (BMP2_OK != (ret = bmp2_get_config(&config, &device))) {
        vlog("Whoops! %s\n", get_bmp2_error(ret));
        goto cleanup;
    }

    config.os_mode = BMP2_OS_MODE_HIGH_RESOLUTION;
    config.odr = BMP2_ODR_125_MS;
    config.filter = BMP2_FILTER_OFF;

    vlog("bmp2_set_power_mode()\n");
    if (BMP2_OK != (ret = bmp2_set_power_mode(BMP2_POWERMODE_FORCED, &config, &device))) {
        vlog("Whoops! %s\n", get_bmp2_error(ret));
        goto cleanup;
    }

    uint32_t measurement_time;

    vlog("bmp2_compute_meas_time()\n");
    if (BMP2_OK != (ret = bmp2_compute_meas_time(&measurement_time, &config, &device))) {
        vlog("Whoops! %s\n", get_bmp2_error(ret));
        goto cleanup;
    }

    int tries = 10;
    struct bmp2_data data;

    while (tries--) {
        struct bmp2_status status;

        vlog("bmp2_get_status()\n");
        if (BMP2_OK != (ret = bmp2_get_status(&status, &device))) {
            vlog("Whoops! %s\n", get_bmp2_error(ret));
            goto cleanup;
        }

        if (status.measuring == BMP2_MEAS_ONGOING || status.im_update == BMP2_IM_UPDATE_ONGOING) {
            printf("Measurement delay : %lu µs\n", (long unsigned int)measurement_time);
            usleep(measurement_time);
            continue;
        }

        vlog("bmp2_get_sensor_data()\n");
        if (BMP2_OK != (ret = bmp2_get_sensor_data(&data, &device))) {
            vlog("Whoops! %s\n", get_bmp2_error(ret));
            goto cleanup;
        }

        break;
    }

    vlog("bmp2_set_power_mode(SLEEP)\n");
    if (BMP2_OK != (ret = bmp2_set_power_mode(BMP2_POWERMODE_SLEEP, &config, &device))) {
        vlog("Whoops! %s\n", get_bmp2_error(ret));
        goto cleanup;
    }

    printf("Temperature: %.4lf deg C\nPressure: %.4lf hPa\n",
           data.temperature,
           data.pressure / 100);

cleanup:
    close(i2c);

    return 0;
}

BMP2_INTF_RET_TYPE bmp2_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    if (write(*(int *)intf_ptr, &reg_addr, 1) != 1) {
        perror("Could not write register to device for read");
        return !BMP2_INTF_RET_SUCCESS;
    }

    if (read(*(int *)intf_ptr, reg_data, length) != length) {
        perror("Could not read from device");
        return !BMP2_INTF_RET_SUCCESS;
    }

    vlog("  Data read starting at register %02X:\n  ", reg_addr);
    for( uint32_t i = 0; i < length; i++) {
        vlog("%02X ", reg_data[i]);
    }
    vlog("\n");

    return BMP2_INTF_RET_SUCCESS;
}

BMP2_INTF_RET_TYPE bmp2_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    uint8_t buffer[10];

    if (length > 9) {
        return !BMP2_INTF_RET_SUCCESS;
    }

    buffer[0] = reg_addr;
    for (uint8_t i = 0; i < length; i++) {
        buffer[i+1] = reg_data[i];
    }

    if (write(*(int *)intf_ptr, buffer, length+1) != length+1) {
        perror("Could not write data to device");
        return !BMP2_INTF_RET_SUCCESS;
    }

    vlog("  Data written:\n  %02X ", reg_addr);
    for( uint32_t i = 0; i < length; i++) {
        vlog("%02X ", reg_data[i]);
    }
    vlog("\n");

    return BMP2_INTF_RET_SUCCESS;
}

void bmp2_delay_us(uint32_t period, __attribute__((unused)) void *intf_ptr)
{
    usleep(period);
}

char *get_bmp2_error(int8_t code)
{
    switch (code) {
        case BMP2_E_NULL_PTR:
            return "NULLs be lurking";
        case BMP2_E_COM_FAIL:
            return "Communication failure";
        case BMP2_E_INVALID_LEN:
            return "Invalid length";
        case BMP2_E_DEV_NOT_FOUND:
            return "BMP280 not found";
        case BMP2_E_UNCOMP_TEMP_RANGE:
            return "Temperature out of range";
        case BMP2_E_UNCOMP_PRESS_RANGE:
            return "Pressure out of range";
        case BMP2_E_UNCOMP_TEMP_AND_PRESS_RANGE:
            return "Both temperature and pressure out of range";
        default:
            return "Don't panic (PANIC)";
    }
}