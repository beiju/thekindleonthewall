#!/usr/bin/env python

def hash(message):
    command = message
    hash = 0

    for i in xrange(len(command)):
        print hash
        print command[i], ord(command[i])
        hash = (ord(command[i]) + (hash << 6) + (hash << 16) - hash) & 0xFFFFFFFF
    return str(hash & 0xFFFFFFFF)


print hash('ledon')
print hash('ledoff')
print hash('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ')
