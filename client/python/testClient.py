#!/usr/bin/env python2.2
import sys
import time
from ircam.fts.client import *

class FtsGraphicObject(FtsObject):
    def __init__(self, server, parent, *args):
        if len(args) == 1:
            if isinstance(args[0], str):
                FtsObject.__init__(self, server, parent, args)
                self.description = args[0]
                print "FtsGraphicObject: description: ", self.description
        elif len(args) == 3:
            FtsObject.__init__(self, server, parent, args[0])
            self.className = args[1]
            self.description = args[2]
            print "FtsGraphicObject: className:", self.className, " description: ", self.description

    def persistence(self, persist):
        self.persist = persist
        print "FtsGraphicObject: persist =", self.persist
        return

    def name(self, name):
        self.varName = name
        print "ftsGraphicObject: varName =", self.varName
        return


class ConsoleStream(FtsObject):
    def __init__(self, connection, rootPatcher):
        FtsObject.__init__(self, connection, rootPatcher, "console_stream")
    
    def print_line(self, args):
        print "Console Stream:", args

class FtsConnection(FtsObject):
    def __init__(self, server, patcher, id, source, outlet, destination, inlet, type):
        FtsObject.__init__(self, server, id)
        self.source = source
        self.outlet = outlet
        self.destination = destination
        self.inlet = inlet
        self.type = type
        print "Created a FtsConnection between:", self.source.description, " outlet:", self.outlet, " and ", \
              self.destination.description, "inlet", self.inlet
        return

        
class FtsPatcher(FtsObject):
    def __init__(self, *args):
        FtsObject.__init__(self, *args)
        self.objects = []
        self.connections = []
        
    def startUpload(self):
        print "FtsPatcher: startUpload"
        return

    def setPatcherBounds(self, x, y, width, height):
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        print "FtsPatcher: setPatcherBounds ", x, y, width, height
        return

    def addObject(self, *args):
        objId = args[0]
        x = args[1]
        y = args[2]
        width = args[3]
        height = args[4]
        numIns = args[5]
        numOuts = args[6]
        layer = args[7]
        errorDescription = args[8]
        className = args[9]
        isTemplate = args[10]

        graphicObject = FtsGraphicObject(self.getServerConnection(), self, objId, className, args[11:])
        self.objects.append(graphicObject)
        return

    def addConnection(self, *args):
        if len(args) == 6:
            connection = FtsConnection(self.getServerConnection(), self, args[0], args[1], args[2], args[3], args[4], args[5])
            self.connections.append(connection)
        return

    def endUpload(self):
        print "FtsPatcher: endUpload"
        return

    def openEditor(self):
        print "FtsPatcher: openEditor"
        return



class ClientPatcher(FtsObject):
    def __init__(self, connection, rootPatcher):
        FtsObject.__init__(self, connection, rootPatcher, 1)
        self.connection = connection
        self.rootPatcher = rootPatcher
    
    def patcher_loaded(self, id, name, type):
        print "Client Patcher: patcher_loaded, id:", id, " name:", name, " type:", type
        # set id
        self.patcher = FtsPatcher(self.connection, self.rootPatcher, id, "jpatcher")
        return

# Etablish connection
connection = FtsSocketConnection()

# Create root patcher
rootPatcher = FtsObject(connection, None, 0)
connection.putObject(0, rootPatcher)

# Create console_stream
console_stream = ConsoleStream(connection, rootPatcher)

console_stream.send("set_default")

clientPatcher = ClientPatcher(connection, rootPatcher);
args = []
# add path to patch to load
args.append(FtsRawString(sys.argv[1]))
# send load message to fts
print "Send message load ", sys.argv[1]
clientPatcher.send("load", args)

# sleep 5 second
time.sleep(5)

# try to shutdown fts
print "Shutdown FTS"
clientPatcher.send("shutdown")

# stop thread and close connection
print "stop thread and close connection"
connection.close()
sys.exit(0)    
