EXEC=i2c_scan pcf8574 mcp9808 ags10_simple bmp280 ssd1306 eeprom_read eeprom_write
LIB=i2c.o options.o getbus.o
CC=gcc
CFLAGS=-O3 -Wall -Wextra -Wpedantic -g

.PHONY:
all: $(EXEC) tags

.ONESHELL:
tags:	*.c *.h
	@if command -V ctags &> /dev/null; then
		ctags *.c *.h
	else
		touch ctags
	fi

.PHONY:
clean:
	rm -f $(EXEC) $(LIB) bmp2.o tags

i2c_scan: i2c_scan.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

pcf8574: pcf8574.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

mcp9808: mcp9808.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

ags10_simple: ags10_simple.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

bmp2.o: bmp2-sensor-api/bmp2.c bmp2-sensor-api/bmp2.h bmp2-sensor-api/bmp2_defs.h
	$(CC) $(CFLAGS) -c -I bmp2-sensor-api bmp2-sensor-api/bmp2.c

bmp280: bmp280.c bmp2.o $(LIB)
	$(CC) $(CFLAGS) -I bmp2-sensor-api -o $@ $^

ssd1306: ssd1306_test.c ssd1306.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lm

eeprom_read: eeprom_read.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

eeprom_write: eeprom_write.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

