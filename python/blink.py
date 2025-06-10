#! /usr/bin/env python3
import time
import board
import digitalio
     
led1 = digitalio.DigitalInOut(board.G1)
led1.direction = digitalio.Direction.OUTPUT

led3 = digitalio.DigitalInOut(board.G3)
led3.direction = digitalio.Direction.OUTPUT
     
while True:
    led1.value = False
    led3.value = True
    time.sleep(0.25)
    led1.value = True
    led3.value = False
    time.sleep(0.25)
