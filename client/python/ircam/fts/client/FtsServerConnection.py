import os
import threading
import BinaryProtocolEncoder
import BinaryProtocolDecoder
from FtsClientException import FtsClientException

class FtsServerConnection:
    """
    Abstract class for client-server connection
    """
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
        """
        Start receive thread
        """
        self.runningThread = 1
        self.readThread.start()

    def stopThread(self):
        """
        Stop receive thread
        """
        self.runningThread = 0
        
    def getNewObjectID(self):
        """
        Return a new object ID
        """
        id = self.__newObjectID
        self.__newObjectID += 2
        return id
    
    def getObject(self, id):
        """
        Return object form an ID
        """
        if self.__objectTable.has_key(id):
            obj = self.__objectTable[id]
        else:
            obj = None
        return obj
    
    def putObject(self,id, obj):
        """
        Add an object in objectTable
        """
        import FtsObject
        if id == FtsObject.FtsObject.NO_ID:
            return FtsObject.FtsObject.NO_ID

        if id == FtsObject.FtsObject.NEW_ID:
            id = self.getNewObjectID()
            
        self.__objectTable[id] = obj
        return id
    
    def writeInt(self, v):
        """
        Add an int in protocol encoder buffer
        """
        self.__encoder.writeInt(v)
        return
    
    def writeDouble(self, v):
        """
        Add a double in protocol encoder buffer
        """
        self.__encoder.writeDouble(v)
        return
    
    def writeString(self, v):
        """
        Add a string in protocol encoder buffer
        """
        self.__encoder.writeString(v)
        return
    
    def writeSymbol(self, v):
        """
        Add a symbol in protocol encoder buffer
        """
        self.__encoder.writeSymbol(v)
        return
    
    def writeRawString(self, v):
        """
        Add a rawstring in protocol encoder buffer
        """
        self.__encoder.writeRawString(v)
        return
    
    def writeObject(self, arg): # Could be a FtsObject or an int
        """
        Add an object in protocol encoder buffer
        """
        self.__encoder.writeObject(arg)
        return
    
    def writeArgs(self, v):
        """
        Add a list of args in protocol encoder buffer
        """
        self.__encoder.writeArgs(v)
        return
    
    def endOfMessage(self):
        """
        Add END_Of_MESSAGE tag in protocol encoder buffer and send
        message to FTS server
        """
        self.__encoder.endOfMessage()
        self.write(self.__encoder.getBytes(), self.__encoder.getLength())
        self.__encoder.clear()
        return
    
    def read(self, byte, length):
        """
        Method to be implemented in derived classes
        """
        raise NotImplementedError
    
    def write(self, byte, length):
        """
        Method to be implemented in derived classes
        """
        raise NotImplementedError
    
    def close(self):
        """
        Method to be implemented in derived classes
        """
        raise NotImplementedError
    
    def receiveThread(self):
        """
        Received thread
        """
        byte = []
        while self.runningThread == 1:
            try:
                n, byte = self.read(byte, FtsServerConnection.DEFAULT_RECEIVE_BUFFER_SIZE)
                self.__decoder.decode(byte)
            except FtsClientException, myex:
                print myex
                self.runningThread = 0
                break



