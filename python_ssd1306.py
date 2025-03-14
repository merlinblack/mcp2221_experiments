#! /usr/bin/env python3
# Basic example of clearing and drawing pixels on a SSD1306 OLED display.
# This example and library is meant to work with Adafruit CircuitPython API.
# Author: Tony DiCola
# License: Public Domain

# Import all board pins.
import board

# Import the SSD1306 module.
import nigel_ssd1306


# Create the I2C interface.
i2c = board.I2C()

# Create the SSD1306 OLED class.
# The first two parameters are the pixel width and pixel height.  Change these
# to the right size for your display!
display = nigel_ssd1306.SSD1306(128, 64, i2c)
# Alternatively you can change the I2C address of the device with an addr parameter:
#display = adafruit_ssd1306.SSD1306_I2C(128, 32, i2c, addr=0x31)

# Clear the display.  Always call show after changing pixels to make the display
# update visible!

print('Draw')
display.fill(0)
display.pixel(0, 0, 1)
display.fill_rect(62, 30, 4, 4, 1)
display.pixel(127, 63, 1)
display.show()
