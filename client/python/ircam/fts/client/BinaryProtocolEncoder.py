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

from SymbolCache import SymbolCache
from BinaryProtocol import BinaryProtocol
from FtsRawString import FtsRawString
from FtsObject import FtsObject

class BinaryProtocolEncoder:
    """
    This class encode message using FTS client-server protocol
    """
    # public:
    # BinaryProtocolEncoder()
    def __init__(self):
        self.__outputBuffer = ""
        self.__symbolCache = SymbolCache()
        return
    
    # void writeInt( int v) throw( FtsClientException);
    def writeInt(self, v):
        """
        Add a int in protocol buffer
        """
        self.__outputBuffer += chr(BinaryProtocol.INT)
        self.write(v)
        return
    
    # void writeDouble( double v) throw( FtsClientException);
    def writeDouble(self, v):
        """
        Add a float in protocol buffer
        """
        self.__outputBuffer += chr(BinaryProtocol.FLOAT)
        raise NotImplementedError
    
    # void writeSymbol( const char *v) throw( FtsClientException);
    def writeSymbol(self, v):
        """
        Add a symbol in protocol
        """
        index = self.__symbolCache.index(v)
        if self.__symbolCache.get(index) == v:
            self.__outputBuffer += chr(BinaryProtocol.SYMBOL_INDEX)
            self.write(index)
        else:
            self.__symbolCache.put(index, v)
            self.__outputBuffer += chr(BinaryProtocol.SYMBOL_CACHE)
            self.write(index)
            self.write(v)
        return
    
    # void writeString( const char *v) throw( FtsClientException);
    def writeString(self, v):
        self.__outputBuffer += chr(BinaryProtocol.STRING)
        self.write(v)
        return
    
    # void writeRawString( const char *v) throw( FtsClientException);
    def writeRawString(self, v):
        """
        Add a rawstring in protocol buffer
        """
        self.__outputBuffer += chr(BinaryProtocol.OBJECT)
        self.write(v)
        return
    
    # void writeObject( const FtsObject *v) throw( FtsClientException);
    def writeObject(self, args):
        """
        Add a FtsObject in protocol buffer
        """
        self.__outputBuffer += chr(BinaryProtocol.OBJECT)
        if type(args) == int:
            self.write(args)
        else:
            if args != None:
                self.write(args.getID())
            else:
                self.write(0)
        return
    
    def writeArgs(self, args):
        """
        Add several args in protocol buffer
        """
        for each in args:
            if type(each) == int:
                self.writeInt(each)
            elif type( each) == float:
                self.writeDouble(each)
            elif type( each) == str:
                self.writeSymbol(each)
            elif isinstance( each, FtsRawString):
                self.writeString(each)
            elif isinstance( each, FtsObject):
                self.writeObject(each)
        return
    
    # void endOfMessage() throw( FtsClientException);
    def endOfMessage(self):
        """
        Add END_OF_MESSAGE tag in protocol buffer
        """
        self.__outputBuffer += chr(BinaryProtocol.END_OF_MESSAGE)
        return
    
    # void clear() { _outputBuffer->clear(); }
    def clear(self):
        """
        Clear protocol buffer
        """
        self.__outputBuffer = ""
        return
    
    # unsigned char *getBytes() { return _outputBuffer->getBytes(); }
    def getBytes(self):
        """
        Return protocol buffer
        """
        return self.__outputBuffer
    
    # int getLength() { return _outputBuffer->getLength(); }
    def getLength(self):
        """
        Return number of elements in protocol buffer
        """
        return len( self.__outputBuffer)
    
    # private:
    # void write( int v);
    def write(self, v):
        """
        Private method for byte swapping
        """
        if type(v) == int or type(v) == long:
            self.__outputBuffer += chr((v >> 24) & 0xFF)
            self.__outputBuffer += chr((v >> 16) & 0xFF)
            self.__outputBuffer += chr((v >> 8) & 0xFF)
            self.__outputBuffer += chr((v >> 0) & 0xFF)
        elif isinstance(v,str):
            self.__outputBuffer += v
            self.__outputBuffer += '\0'          
        return
