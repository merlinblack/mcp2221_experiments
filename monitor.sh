#!/usr/bin/env bash

touch ~/noclock

trap "rm ~/noclock" EXIT

while true; do

  BMP=$(bmp280 --json)
  AHT=$(aht10 --json)
  BH=$(bh1750 --json)

  TEMPERATURE=$(echo $BMP | jq .temperature)
  HUMIDITY=$(echo $AHT | jq .humidity)
  PRESSURE=$(echo $BMP | jq .pressure)
  LUX=$(echo $BH | jq .lux)

  oled -t3 -c Temperature -k32 $TEMPERATURE
  sleep 5
  oled -t3 -c Humidity -k32 $HUMIDITY
  sleep 5
  oled -t3 -c Pressure -k32 $PRESSURE
  sleep 5
  oled -t3 -c Lux -k32 $LUX
  sleep 5

done
