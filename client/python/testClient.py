#!/usr/bin/env python2.2
import sys
import time
from ircam.fts.client import *


class ConsoleStream(FtsObject):
    def __init__(self, connection, rootPatcher):
        FtsObject.__init__(self, connection, rootPatcher, "console_stream")
    
    def print_line(self, args):
        print "Console Stream:", args
        

# Etablish connection
connection = FtsSocketConnection()

# Create root patcher
rootPatcher = FtsObject(connection, None, 0)
connection.putObject(0, rootPatcher)

# Create console_stream
console_stream = ConsoleStream(connection, rootPatcher)

console_stream.send("set_default")

clientPatcher = FtsObject(connection, rootPatcher, 1);
args = []
# add path to patch to load
args.append(FtsRawString(sys.argv[1]))
# send load message to fts
print "Send message load ", sys.argv[1]
clientPatcher.send("load", args)

# sleep one second
time.sleep(1)

connection.close()
sys.exit(0)    
