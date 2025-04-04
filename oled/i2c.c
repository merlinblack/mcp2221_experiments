#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

int i2c_open(int bus, int address)
{
  int file;
  char filename[20];

  snprintf(filename, 19, "/dev/i2c-%d", bus);

  file = open(filename, O_RDWR);
  if (file < 0) {
    perror("Error opening i2c bus");
    return file;
  }

  if (address < 0) {
    return file;
  }

  if (ioctl(file, I2C_SLAVE, address) > 0) {
    perror("Error selecting device");
    close(file);
    return -1;
  }

  return file;
}

int i2c_select(int file, int address)
{
  if (ioctl(file, I2C_SLAVE, address) > 0) {
    return -1;
  }

  return 0;
}