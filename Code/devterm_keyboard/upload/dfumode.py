#!/usr/bin/python

import time
import serial
import serial.tools.list_ports as lports
import argparse


def main(args):

    # Find which serial port to use (assumes only the keyboard is connected)
    if args.port == '':
        port_list = lports.comports()
        if len(port_list) != 1:
            print('No serial ports detected, quitting...')
            quit()
        else:
            port = port_list[0].device
    else:
        port = args.port

    # Based on https://github.com/rogerclarkmelbourne/Arduino_STM32/blob/master/tools/linux/src/upload-reset/upload-reset.c
    # Send magic sequence of DTR and RTS followed by the magic word "1EAF"
    with serial.Serial(port=port, baudrate=args.baudrate) as ser:

        ser.rts = False
        ser.dtr = False
        ser.dtr = True

        time.sleep(0.05)

        ser.dtr = False
        ser.rts = True
        ser.dtr = True

        time.sleep(0.05)

        ser.dtr = False

        time.sleep(0.05)

        ser.write(b'1EAF')

    # Wait for the DFU mode reboot
    time.sleep(args.sleep * 1e-3)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser = argparse.ArgumentParser(description="Enter DFU mode of DevTerm's keyboard.")
    parser.add_argument('-p', '--port', dest='port', type=str, default='',
                        help='Serial port (default: detect serial ports).')
    parser.add_argument('-b', '--baudrate', dest='baudrate', type=int, default=9600,
                        help='Baudrate (default: 9600bps).')
    parser.add_argument('-s', '--sleep', dest='sleep', type=int, default=1500,
                        help='Wait SLEEP milliseconds (ms) after enabling DFU mode (default: 1500ms).')
    args = parser.parse_args()

    main(args)
