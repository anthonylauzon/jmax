import os
import threading
#from ircam.fts.client.BinaryProtocolEncoder import BinaryProtocolEncoder
import BinaryProtocolEncoder
#from ircam.fts.client.BinaryProtocolDecoder import BinaryProtocolDecoder
import BinaryProtocolDecoder

class FtsServerConnection:
    DEFAULT_RECEIVE_BUFFER_SIZE = 65536
    CLIENT_OBJECT_ID = 1
    
    def __init__(self):
        self.__encoder = BinaryProtocolEncoder.BinaryProtocolEncoder()
        self.__decoder = BinaryProtocolDecoder.BinaryProtocolDecoder(self)
        self.__newObjectID = 16
        self.__objectTable = {}
        self.readThread = threading.Thread(target=self.receiveThread)
        raise NotImplementedError

    def startThread(self):
        self.runningThread = 1
        self.readThread.start()

    def stopThread(self):
        self.runningThread = 0
        
    def getNewObjectID(self):
        id = self.__newObjectID
        self.__newObjectID += 2
        return id
    
    def getObject(self, id):
        if self.__objectTable.has_key(id):
            obj = self.__objectTable[id]
        else:
            obj = None
        return obj
    
    def putObject(self,id, obj):
        import FtsObject
        if id == FtsObject.FtsObject.NO_ID:
            return FtsObject.FtsObject.NO_ID

        if id == FtsObject.FtsObject.NEW_ID:
            id = self.getNewObjectID()
            
        self.__objectTable[id] = obj
        return id
    
    def writeInt(self, v):
        self.__encoder.writeInt(v)
        return
    
    def writeDouble(self, v):
        self.__encoder.writeDouble(v)
        return
    
    def writeString(self, v):
        self.__encoder.writeString(v)
        return
    
    def writeSymbol(self, v):
        self.__encoder.writeSymbol(v)
        return
    
    def writeRawString(self, v):
        self.__encoder.writeRawString(v)
        return
    
    def writeObject(self, arg): # Could be a FtsObject or an int
        self.__encoder.writeObject(arg)
        return
    
    def writeAtoms(self, atoms):        # tuple of atoms
        self.__encoder.writeAtoms(atoms)
        return
    
    def writeArgs(self, v):
        self.__encoder.writeArgs(v)
        return
    
    def endOfMessage(self):
        self.__encoder.endOfMessage()
        self.write(self.__encoder.getBytes(), self.__encoder.getLength())
        self.__encoder.clear()
        return
    
    def read(self, byte, length):
        raise NotImplementedError
    
    def write(self, byte, length):
        raise NotImplementedError
    
    def close(self):
        raise NotImplementedError
    
    def receiveThread(self):
        byte = []
        while self.runningThread == 1:
            n, byte = self.read(byte, FtsServerConnection.DEFAULT_RECEIVE_BUFFER_SIZE)
            if n < 0:
                raise FtsClientException("Failed to read the input connection", os.errno)
            if n == 0:
                raise FtsClientException("End of input", os.errno)
            self.__decoder.decode(byte)
        

