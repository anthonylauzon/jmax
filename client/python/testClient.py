#!/usr/bin/env python2.2
import sys
import time
from ircam.fts.client import *


class ConsoleStreamHandler:
    def invoke(self, obj, args):
        print "Console Stream:", args.getString(0)

# Etablish connection
connection = FtsSocketConnection()

# Create root patcher
rootPatcher = FtsObject(connection, None, 0)
connection.putObject(0, rootPatcher)

# Create console_stream
console_stream = FtsObject(connection, rootPatcher, "console_stream")

handler = ConsoleStreamHandler()
# Register Handler .....
FtsObject.registerMessageHandler(console_stream.__class__, "print_line", handler);

console_stream.send("set_default")

clientPatcher = FtsObject(connection, rootPatcher, 1);
args = FtsArgs()
args.clear()
# add path to patch to load
args.addString(sys.argv[1])
# send load message to fts
clientPatcher.send("load", args)

# sleep one second
time.sleep(1)

connection.close()
sys.exit(0)    
