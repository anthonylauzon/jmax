from SymbolCache import SymbolCache
from BinaryProtocol import BinaryProtocol
from FtsRawString import FtsRawString

class BinaryProtocolEncoder:
    # public:
    # BinaryProtocolEncoder()
    def __init__(self):
        self.__outputBuffer = ""
        self.__symbolCache = SymbolCache()
        return
    
    # void writeInt( int v) throw( FtsClientException);
    def writeInt(self, v):
        self.__outputBuffer += chr(BinaryProtocol.INT)
        self.write(v)
        return
    
    # void writeDouble( double v) throw( FtsClientException);
    def writeDouble(self, v):
        self.__outputBuffer += chr(BinaryProtocol.FLOAT)
        raise NotImplementedError
    
    # void writeSymbol( const char *v) throw( FtsClientException);
    def writeSymbol(self, v):
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
        self.__outputBuffer += chr(BinaryProtocol.OBJECT)
        self.write(v)
        return
    
    # void writeObject( const FtsObject *v) throw( FtsClientException);
    def writeObject(self, args):
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
        self.__outputBuffer += chr(BinaryProtocol.END_OF_MESSAGE)
        return
    
    # void clear() { _outputBuffer->clear(); }
    def clear(self):
        self.__outputBuffer = ""
        return
    
    # unsigned char *getBytes() { return _outputBuffer->getBytes(); }
    def getBytes(self):
        return self.__outputBuffer
    
    # int getLength() { return _outputBuffer->getLength(); }
    def getLength(self):
        return len( self.__outputBuffer)
    
    # private:
    # void write( int v);
    def write(self, v):
        if type(v) == int or type(v) == long:
            self.__outputBuffer += chr((v >> 24) & 0xFF)
            self.__outputBuffer += chr((v >> 16) & 0xFF)
            self.__outputBuffer += chr((v >> 8) & 0xFF)
            self.__outputBuffer += chr((v >> 0) & 0xFF)
        elif isinstance(v,str):
            self.__outputBuffer += v
            self.__outputBuffer += '\0'          
        return
