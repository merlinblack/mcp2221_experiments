EXEC=i2c_scan aht10 bh1750 pcf8574 mcp9808 ags10 bmp280 ssd1306 eeprom_read eeprom_write
LIB=i2c.o options.o getbus.o
CC=gcc
CFLAGS-std=gnu23 =-O3 -Wall -Wextra -Wpedantic -g
INSTALLDIR=$(HOME)/.local/bin

.PHONY:
all: $(EXEC)

.PHONY:
clean:
	rm -f $(EXEC) *.o

aht10: aht10.o aht10_cli.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

bh1750: bh1750.o bh1750_cli.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

i2c_scan: i2c_scan.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

pcf8574: pcf8574.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

mcp9808: mcp9808.o mcp9808_cli.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

ags10: ags10.o ags10_cli.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

bmp2.o: bmp2-sensor-api/bmp2.c bmp2-sensor-api/bmp2.h bmp2-sensor-api/bmp2_defs.h
	$(CC) $(CFLAGS) -c -I bmp2-sensor-api bmp2-sensor-api/bmp2.c

bmp280.o: bmp280.c
	$(CC) $(CFLAGS) -c -I bmp2-sensor-api -o $@ $^

bmp280: bmp280.o bmp280_cli.o bmp2.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

ssd1306: ssd1306_test.o ssd1306.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lm

eeprom_read: eeprom_read.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

eeprom_write: eeprom_write.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

install: mcp9808 bmp280 ags10 aht10 bh1750
	install mcp9808 $(INSTALLDIR)
	install bmp280 $(INSTALLDIR)
	install ags10 $(INSTALLDIR)
	install aht10 $(INSTALLDIR)
	install bh1750 $(INSTALLDIR)
