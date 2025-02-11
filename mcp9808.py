#!/usr/bin/env python3

import time
import board
import adafruit_mcp9808
import digitalio

i2c = board.I2C()  # uses board.SCL and board.SDA

mcp = adafruit_mcp9808.MCP9808(i2c)

led0 = digitalio.DigitalInOut(board.G0)
led0.direction = digitalio.Direction.OUTPUT

led1 = digitalio.DigitalInOut(board.G1)
led1.direction = digitalio.Direction.OUTPUT

led3 = digitalio.DigitalInOut(board.G3)
led3.direction = digitalio.Direction.OUTPUT
     

led1.value = False
led3.value = True

while True:
    led0.value = True
    led1.value = not led1.value
    led3.value = not led3.value
    tempC = mcp.temperature
    tempF = tempC * 9 / 5 + 32
    print("Temperature: {} C {} F ".format(tempC, tempF))
    led0.value = False
    time.sleep(2)

