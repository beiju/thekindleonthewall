#!/usr/bin/env python

from time import sleep
import serial

FILENAME = '/dev/ttyACM0'
BAUD = 115200
TIMEOUT = 0
SIGNATURE = 'Dog'


def parse_command(message):
    args = message.split(' ')
    signature = args.pop(0)
    if signature != SIGNATURE:
        return False, None, None
    check = args.pop()

    raw_command = ' '.join(args)
    command = args.pop(0)

    hash = 0
    for i in xrange(len(raw_command)):
        hash = (ord(raw_command[i]) + (hash << 6) + (hash << 16) - hash) & 0xFFFFFFFF
    return (str(hash & 0xFFFFFFFF) == check), command, args


def run():
    ser = serial.Serial(FILENAME, BAUD, timeout=TIMEOUT)
    while True:
        message = ser.readline().strip()
        valid, command, args = parse_command(message)
        if message != "":
            if not valid:
                print "Invalid message"
            print message
        sleep(.01)
    

if __name__ == '__main__':
    run()
