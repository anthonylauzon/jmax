from Buffer import Buffer
from SymbolCache import SymbolCache
from BinaryProtocol import BinaryProtocol

class BinaryProtocolEncoder:
    # public:
    # BinaryProtocolEncoder()
    def __init__(self):
        self.__outputBuffer = Buffer()
        self.__symbolCache = SymbolCache()
        return
    
    # void writeInt( int v) throw( FtsClientException);
    def writeInt(self, v):
        self.__outputBuffer.append(chr(BinaryProtocol.INT))
        self.write(v)
        return
    
    # void writeDouble( double v) throw( FtsClientException);
    def writeDouble(self, v):
        self.__outputBuffer.append(chr(BinaryProtocol.FLOAT))
        raise NotImplementedError
    
    # void writeSymbol( const char *v) throw( FtsClientException);
    def writeSymbol(self, v):
        index = self.__symbolCache.index(v)
        if self.__symbolCache.get(index) == v:
            self.__outputBuffer.append(chr(BinaryProtocol.SYMBOL_INDEX))
            self.write(v)
        else:
            self.__symbolCache.put(index, v)
            self.__outputBuffer.append(chr(BinaryProtocol.SYMBOL_CACHE))
            self.write(index)
            self.write(v)
        return
    
    # void writeString( const char *v) throw( FtsClientException);
    def writeString(self, v):
        self.__outputBuffer.append(chr(BinaryProtocol.STRING))
        self.write(v)
        return
    
    # void writeRawString( const char *v) throw( FtsClientException);
    def writeRawString(self, v):
        self.__outputBuffer.append(chr(BinaryProtocol.OBJECT))
        self.write(v)
        return
    
    # void writeObject( const FtsObject *v) throw( FtsClientException);
    def writeObject(self, args):
        self.__outputBuffer.append(chr(BinaryProtocol.OBJECT))
        if type(args) == int:
            self.write(args)
        else:
            if args != None:
                self.write(args.getID())
            else:
                self.write(0)
        return
    
    #void writeAtoms( const FtsAtom *atoms, int length) throw (FtsClientException);
    def writeAtoms(self, atoms):
        for each in atoms:
            if each.isInt():
                self.writeInt(each.value)
            elif each.isDouble():
                self.writeDouble(each.value)
            elif each.isSymbol():
                self.writeSymbol(each.value)
            elif each.isString():
                self.writeString(each.value)
            elif each.isRawString():
                self.writeString(each.value)
            elif each.isObject():
                self.writeString(each.value)
        
        return
    
    # void writeArgs( const FtsArgs &v) throw( FtsClientException);
    def writeArgs(self, args):
        self.writeAtoms(args.getAtoms())
        return
    
    # void endOfMessage() throw( FtsClientException);
    def endOfMessage(self):
        self.__outputBuffer.append(chr(BinaryProtocol.END_OF_MESSAGE))
        return
    
    # void clear() { _outputBuffer->clear(); }
    def clear(self):
        self.__outputBuffer.clear()
        return
    
    # unsigned char *getBytes() { return _outputBuffer->getBytes(); }
    def getBytes(self):
        return self.__outputBuffer.getBytes()
    
    # int getLength() { return _outputBuffer->getLength(); }
    def getLength(self):
        return self.__outputBuffer.getLength()
    
    # private:
    # void write( int v);
    def write(self, v):
        if type(v) == int or type(v) == long:
            self.__outputBuffer.append(chr((v >> 24) & 0xFF))
            self.__outputBuffer.append(chr((v >> 16) & 0xFF))
            self.__outputBuffer.append(chr((v >> 8) & 0xFF))
            self.__outputBuffer.append(chr((v >> 0) & 0xFF))
        elif type(v) == str:
            self.__outputBuffer.append(v)
            self.__outputBuffer.append('\0')          
        return
