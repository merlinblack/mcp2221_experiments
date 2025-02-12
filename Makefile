EXEC=i2c_scan pcf8574 mcp9808
CC=gcc

.PHONY:
all: $(EXEC)

.PHONY:
clean:
	rm $(EXEC) i2c.o

i2c_scan: i2c_scan.c i2c.o
	gcc -o $@ $^

pcf8574: pcf8574.c i2c.o
	gcc -o $@ $^

mcp9808: mcp9808.c i2c.o
	gcc -o $@ $^

