EXEC=i2c_scan pcf8574 mcp9808 ags10_simple
LIB=i2c.o getbus.o
CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic

.PHONY:
all: $(EXEC)

.PHONY:
clean:
	rm $(EXEC) $(LIB)

i2c_scan: i2c_scan.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

pcf8574: pcf8574.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

mcp9808: mcp9808.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

ags10_simple: ags10_simple.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

