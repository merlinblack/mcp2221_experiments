#CFLAGS=-std=gnu23 -g
CFLAGS=-std=gnu23 -O3

COMMON=options.c ssd1306.c i2c.c
COMMON_OBJ=$(COMMON:.c=.o)
LIBS=

INSTALLDIR=$(HOME)/.local/bin

.PHONY: all
all: oled oledinit oledinvert oleddim clock clockd

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

oled: oled.c $(COMMON_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clock: clock.c $(COMMON_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clockd: clockd.c $(COMMON_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

oledinit: init.c $(COMMON_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

oledinvert: invert.c $(COMMON_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

oleddim: dim.c $(COMMON_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f *.o
	rm -f oled clock clockd oledinit oledinvert oleddim

.PHONY: install
install: all
	install oled $(INSTALLDIR)
	install clock $(INSTALLDIR)
	install clockd $(INSTALLDIR)
	install oledinit $(INSTALLDIR)
	install oledinvert $(INSTALLDIR)
	install oleddim $(INSTALLDIR)
