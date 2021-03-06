#
# jMax
# Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser Lesser General Public License
# as published by the Free Software Foundation; either version 2.1
# of the License, or (at your option) any later version.
# 
# See file COPYING.LIB for further informations on licensing terms.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser Lesser General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 

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
        self.readThread._stopevent = threading.Event()
        raise NotImplementedError

    def startThread(self):
        """
        Start receive thread
        """
        self.readThread.start()

    def stopThread(self):
        """
        Stop receive thread
        """
        self.readThread._stopevent.set()
        # threading.Thread.join(self.readThread)
    
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
        while not self.readThread._stopevent.isSet():
            try:
                n, byte = self.read(byte, FtsServerConnection.DEFAULT_RECEIVE_BUFFER_SIZE)
                self.__decoder.decode(byte)
            except FtsClientException, myex:
                # close the connection if we can't read ...
                self.close()



