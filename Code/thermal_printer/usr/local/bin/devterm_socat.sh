#!/bin/bash


sleep 4
chmod 777 /tmp/DEVTERM_PRINTER_IN
chmod 777 /tmp/DEVTERM_PRINTER_OUT

DC=`find /sys/bus/iio/devices/iio\:device{0,1}  | wc -l`

if [ "$DC" -gt "1" ];
then
echo "adc node err"
else
ln -s /sys/bus/iio/devices/iio\:device*/in_voltage*_raw /tmp/devterm_adc
fi

