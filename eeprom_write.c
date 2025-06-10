#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "getbus.h"
#include "i2c.h"

bool write_eeprom_chunk(int i2c, uint16_t address, uint8_t* data, size_t size)
{
  printf("Writing %p -> %X %ld (0x%lX) bytes\n", data, address, size, size);

  uint8_t buffer[2 + 64];  // 2 bytes address + max chunk

  // Most significant byte first - AT24C32 datasheet.
  // Intresting note:
  // The '& 0xff' is not really required, a right shift on unsigned will fill
  // the vaccant positions with bits equal to zero.
  // However the compiler (I'm using gcc) generates slightly smaller code
  // when you have it, taking advantage of the register 'al'
  // being the low byte of 'ax', and not doing a shift at all.
  buffer[0] = (address >> 8) & 0xff;
  buffer[1] = address & 0xff;

  memcpy(&buffer[2], data, size);

  if (write(i2c, buffer, size + 2) != (ssize_t)size + 2) {
    return true;
  }

  return false;
}

bool write_eeprom(int i2c, uint16_t address, uint8_t* data, size_t size)
{
  size_t written = 0;
  const uint8_t alignment = 32;
  const uint8_t max_chunk_size = 32;

  // First aligned address after address
  uint16_t aligned = (address / alignment + 1) * alignment;

  if (write_eeprom_chunk(i2c, address, data, aligned - address)) {
    return true;
  }
  written = aligned - address;
  address += written;

  while (written < size) {
    uint8_t chunk_size =
        size - written > max_chunk_size ? max_chunk_size : size - written;
    if (write_eeprom(i2c, address, data + written, chunk_size)) {
      return true;
    }
    address += chunk_size;
    written += chunk_size;

    usleep(10);
  }

  return false;
}

int main(int argc, char** argv)
{
  int bus = get_bus(argc, argv);
  int device_address = 0x52;  // eeprom address

  int i2c = i2c_open(bus, device_address);

  if (i2c < 0) {
    return 0;
  }

  // This data is on my test eeprom - below test data should not smash it.
  /*
  uint8_t address_and_data[8] = { 0, 0, 'A', 's', 'l', 'a', 'n', 0 };
  if (write(i2c, address_and_data, 8) != 8) {
      perror("EEPROM Write address and data");
      goto cleanup;
  }
  */

  uint8_t data[0x1000];
  size_t size;

  // Test data.
  int fd = open("./eeprom_write.c", O_RDONLY);
  size = read(fd, data, 0x1000);
  close(fd);
  data[++size] = 0;

  if (write_eeprom(i2c, 6, data, size)) {
    perror("Error writing to eeprom.");
    goto cleanup;
  }

  printf("Written: %zu (0x%zX) bytes.\n", size, size);

cleanup:
  close(i2c);

  return 0;
}
