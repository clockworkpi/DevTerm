#!/bin/bash

sleep 4
chmod 777 /tmp/DEVTERM_PRINTER_IN
chmod 777 /tmp/DEVTERM_PRINTER_OUT

ln -s /sys/bus/iio/devices/iio\:device*/in_voltage*_raw /tmp/devterm_adc
