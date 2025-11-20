#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "getbus.h"
#include "i2c.h"

int main(int argc, char** argv)
{
  int bus = get_bus(argc, argv);

  int i2c = i2c_open(bus, -1);

  if (i2c < 0) {
    return EXIT_FAILURE;
  }

  printf("Write test i2c scan...\n");
  printf("      0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
  printf("                            ");

  uint8_t data[1] = {0x00};

  for (int address = 0x08; address < 0x78; address++) {
    if (i2c_select(i2c, address) == -1) {
      if (errno == EBUSY) {
        printf(" UU");
      }
      else {
        perror("Well this never happens? ");
        break;
      }
    }

    if (address % 0x10 == 0) {
      printf("\n%02x: ", address);
    }

    // "Quick" write - i.e. just the address sent out
    if (write(i2c, data, 0) == 0) {
      // Found a device.
      printf(" %02x", address);
    }
    else {
      printf(" --");
    }
  }

  printf("\n");

  close(i2c);

  return EXIT_SUCCESS;
}
