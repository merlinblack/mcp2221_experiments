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

  oled -t2 $TEMPERATURE $HUMIDITY $PRESSURE $LUX

  sleep 2

done
