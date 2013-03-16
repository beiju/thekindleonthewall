#!/usr/bin/env python

import serial

# Serial configuration
SIGNATURE = 'Dog'
FILENAME = '/dev/ttyACM0'
BAUD = 115200
TIMEOUT = 1  # seconds


class Utils:
    @classmethod
    def checksum(cls, message):
        hash = 0
        for i in xrange(len(message)):
            hash = ord(message[i]) + (hash << 6) + (hash << 16) - hash
        return str(hash & 0xFFFFFFFF)


class Arduino:
    def __init__(self):
        self.com = serial.Serial(FILENAME, BAUD, timeout=TIMEOUT)
        try:
            self.com.open()
        except:
            pass

    def send_raw_command(self, raw_command):
        checksum = Utils.checksum(raw_command)
        output = '%s %s %s\r\n' % (SIGNATURE, raw_command, checksum)
        print output
        self.com.write(output)
        
    def send_command(self, command_name, *args):
        send_raw_command(' '.join((command_name,) + args))

    def toggle_lights(self, number):
        self.send_command('togglelight', (number,))

    def close(self):
        self.com.close()
