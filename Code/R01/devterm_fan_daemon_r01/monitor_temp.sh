#!/bin/bash


THRESHOLD_HIGH=70000
THRESHOLD_LOW=70000  

FAN_INIT="sudo gpio mode 41 out"
PROGRAM_HIGH="sudo gpio write 41 1"
PROGRAM_LOW="sudo gpio write 41 0"

SLEEP_VALUE=10

$FAN_INIT

while true; do
  for zone in /sys/class/thermal/thermal_zone*; do
    if [ -e "$zone/temp" ]; then
      temp=$(cat "$zone/temp")
      echo "Current temperature from $zone: $(($temp / 1000))°C"

      if [ "$temp" -ge "$THRESHOLD_HIGH" ]; then
        echo "Temperature exceeds high threshold: $(($temp / 1000))°C"
        echo "Running high temp program: $PROGRAM_HIGH"
        $PROGRAM_HIGH 
        SLEEP_VALUE=30 
      elif [ "$temp" -le "$THRESHOLD_LOW" ]; then
        echo "Temperature below low threshold: $(($temp / 1000))°C"
        echo "Running low temp program: $PROGRAM_LOW"
        $PROGRAM_LOW 
      fi
    fi
  done
  sleep $SLEEP_VALUE
  SLEEP_VALUE=10
done
