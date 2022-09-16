#!/bin/bash

altID="2"
usbID="1EAF:0003"
serial_port="/dev/ttyACM0"

# Default export location
# binfile="../devterm_keyboard.ino.generic_stm32f103r8.bin"
binfile="devterm_keyboard.ino.bin"

# Send magic numbers via serial to enter DFU mode.
# This needs pyserial, so it's installed quietly:
# pip install -q pyserial
./dfumode.py -p $serial_port -b 9600 -s 1500

# Alternatively you can compile the C program that does the same thing:
# gcc upload-reset.c -o upload-reset
# ./upload-reset $serial_port 1500

# Upload binary file
dfu-util -d ${usbID} -a ${altID} -D ${binfile} -R

echo "Waiting for $serial_port serial..."
COUNTER=0
while [ ! -c $serial_port ] && ((COUNTER++ < 40)); do
    sleep 0.1
done
echo Done


