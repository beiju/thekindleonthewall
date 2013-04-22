#!/usr/bin/env python

"""Simple class for encapsulating sending raw commands to an Arduino over
a serial connection. Configurable via settings.py"""

import serial
from settings import FILENAME, BAUD, SIGNATURE, TIMEOUT


class Utils:
    @classmethod
    def checksum(cls, message):
        hash = 0
        for i in xrange(len(message)):
            hash = ord(message[i]) + (hash << 6) + (hash << 16) - hash
        return str(hash & 0xFFFFFFFF)


class Arduino:
    def __init__(self, signature, filename, baud, timeout):
        self.signature = signature
        self.filename = filename
        self.baud = baud
        self.timeout = timeout
        self.commands = dict()
        self.message = ""
        
        self.open()
    
    def add_command(self, command, function):
        self.commands[command] = function;
    
    def command(self, command):
        def decorator(function):
            self.add_command(command, function)
        return decorator
    
    def refresh(self):
        try:
            while self.connected and self.com.inWaiting() > 0:
                self.message += self.com.readline(self.com.inWaiting())
                if self.message[-1] == '\n' and self.message.strip() != "":
                    self.message = self.message.strip()
                    #! Debug message passthrough - make this nicer
                    prefix = self.message.split()[0]
                    if "DEBUG" in prefix or "INFO" in prefix or "ERROR" in prefix or "WARNING" in prefix:
                        print self.message
                    else:
                        valid, command, args = self.parse_command(self.message)
                        if not valid:
                            print "Recieved invalid message:", self.message
                        elif command in self.commands:
                            self.commands[command](args)
                        else:
                            print "Recieved unknown command:", command, "args:", args
                    self.message = ""
        except IOError:
            self.connected = False
            print "Arduino disconnected!"

    def send_raw_command(self, raw_command):
        if self.connected:
            checksum = Utils.checksum(raw_command)
            output = '%s %s %s\r\n' % (self.signature, raw_command, checksum)
            print output
            self.com.write(output)
        
    def send_command(self, command_name, *args):
        if self.connected:
            self.send_raw_command(' '.join((command_name,) + args))
    
    def parse_command(self, msg):
        args = msg.split(' ')
        signature = args.pop(0)
        if signature == "DOG:DEBUG":
            print msg
        if signature != SIGNATURE:
            return False, None, None
        check = args.pop()

        raw_command = ' '.join(args)
        command = args.pop(0)

        hash = 0
        for i in xrange(len(raw_command)):
            hash = (ord(raw_command[i]) + (hash << 6) + (hash << 16) - hash) & 0xFFFFFFFF
        return (str(hash & 0xFFFFFFFF) == check), command, args

    def toggle_lights(self, number):
        if connected:
            self.send_command('togglelight', (number,))

    def close(self):
        self.com.close()
        self.connected = False

    def open(self):
        try:
            self.com.open()
            self.connected = True
        except AttributeError:
            try:
                self.com = serial.Serial(self.filename, self.baud, timeout=self.timeout)
                self.connected = True
            except serial.serialutil.SerialException:
                self.connected = False
        except serial.serialutil.SerialException:
            self.connected = False