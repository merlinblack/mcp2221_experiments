EXEC=i2c_scan pcf8574 mcp9808

.PHONY:
all: $(EXEC)

.PHONY:
clean:
	rm $(EXEC)

i2c_scan: i2c_scan.c
	gcc -o $@ $?

pcf8574: pcf8574.c
	gcc -o $@ $?

mcp9808: mcp9808.c
	gcc -o $@ $?

