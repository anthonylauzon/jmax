# debug
from Buffer import Buffer
from SymbolCache import SymbolCache
from BinaryProtocol import BinaryProtocol
#from FtsArgs import FtsArgs
import FtsArgs
from FtsClientException import FtsClientException
from HashTable import HashTable

qInitial = 1

qInt0 = 10
qInt1 = 11
qInt2 = 12
qInt3 = 13

qFloat0 = 20
qFloat1 = 21
qFloat2 = 22
qFloat3 = 23
qFloat4 = 24
qFloat5 = 25
qFloat6 = 26
qFloat7 = 27

qString = 30

qObject0 = 40
qObject1 = 41
qObject2 = 42
qObject3 = 43

qSymbolIndex0 = 50
qSymbolIndex1 = 51
qSymbolIndex2 = 52
qSymbolIndex3 = 53

qSymbolCache0 = 60
qSymbolCache1 = 61
qSymbolCache2 = 62
qSymbolCache3 = 63
qSymbolCache4 = 64

class BinaryProtocolDecoder:
    symbolTable = HashTable()
    
    def __init__(self, serverConnection):
        self.__serverConnection = serverConnection
        self.__argsCount = 0
        self.__currentState = qInitial
        self.__lval = 0
        self.__buffer = Buffer()
        self.__selector = ""
        self.__args = FtsArgs.FtsArgs()
        self.__symbolCache = SymbolCache()

    def getState(self):
        return self.__currentState
    
    def clearAction(self, input):
        self.__lval = 0
    
    def shiftAction(self, input):
        self.__lval = self.__lval << 8 | input
    
    def shiftLongAction(self, input):
        self.__lval = self.__lval << 8 | input
    
    def bufferClearAction(self, input):
        self.__buffer.clear()
    
    def clearAllAction(self, input):
        self.__lval = 0
        self.__buffer.clear()

    def bufferShiftAction(self, input):
        self.__buffer.append(chr(input))
    
    def endIntAction(self, input):
        self.__lval = self.__lval << 8 | input
        if self.__argscount >= 2:
            self.__args.addInt(self.__lval)
        self.__argsCount += 1
    
    def endFloatAction(self, input):
        self.__lval = self.__lval << 8 | input
        if self.__argsCount >= 2:
            self.__args.addDouble(3.14)
        self.__argsCount += 1
    
    def endSymbolIndexAction(self, input):
        self.__lval = self.__lval << 8 | input
        s = self.__symbolCache.get(self.__lval)
        if self.__argsCount == 1:
            self.__selector = s
        else:
            self.__args.addSymbol(s)
        self.__argsCount += 1
    
    def endSymbolCacheAction(self, input):
        p = self.__buffer.getBytes()
        status, symbol = BinaryProtocolDecoder.symbolTable.get(p)
        if status == 0:
            symbol = p
            BinaryProtocolDecoder.symbolTable.put(symbol, symbol)
        
        BinaryProtocolDecoder.symbolTable.put(symbol, symbol)

        self.__symbolCache.put(self.__lval, symbol)

        if self.__argsCount == 1:
            self.__selector = symbol
        else:
            self.__args.addSymbol(symbol)
        self.__argsCount += 1
    
    def endStringAction(self, input):
        if self.__argsCount >= 2:
            self.__args.addString(self.__buffer.getBytes())
        self.__argsCount += 1
    
    def endObjectAction(self, input):
        self.__lval = self.__lval << 8 | input
        obj = self.__serverConnection.getObject(self.__lval)
        if self.__argsCount == 0:
            self.__target = obj
        else:
            self.__args.addObject(obj)
        
        self.__argsCount += 1
    
    def endMessageAction(self, input):
        from FtsObject import FtsObject
        FtsObject.invokeMessageHandler(self.__target, self.__selector, self.__args)
        self.__args.clear()
        self.__argsCount = 0
    
    def nextState(self, input):
        if self.__currentState == 0:
            if input == BinaryProtocol.END_OF_MESSAGE:
                self.__currentState = qInitial
        elif self.__currentState == qInitial:
            if input == BinaryProtocol.INT:
                self.__currentState = qInt0
                self.clearAction(input)
            elif input == BinaryProtocol.FLOAT:
                self.__currentState = qFloat0
                self.clearAction(input)
            elif input == BinaryProtocol.SYMBOL_INDEX:
                self.__currentState = qSymbolIndex0
                self.clearAction(input)
            elif input == BinaryProtocol.SYMBOL_CACHE:
                self.__currentState = qSymbolCache0
                self.clearAllAction(input)
            elif input == BinaryProtocol.STRING:
                self.__currentState = qString
                self.bufferClearAction(input)
            elif input == BinaryProtocol.OBJECT:
                self.__currentState = qObject0
                self.clearAction(input)
            elif input == BinaryProtocol.END_OF_MESSAGE:
                self.endMessageAction(input)
            else:
                self.__currentState =0
        
        elif self.__currentState == qInt0:
            self.__currentState = qInt1
            self.shiftAction(input)
        elif self.__currentState == qInt1:
            self.__currentState = qInt2
            self.shiftAction(input)
        elif self.__currentState == qInt2:
            self.__currentState = qInt3
            self.shiftAction(input)
        elif self.__currentState == qInt3:
            self.__currentState = qInitial
            self.endIntAction(input)
        elif self.__currentState == qFloat0:
            self.__currentState = qFloat1
            self.shiftAction(input)
        elif self.__currentState == qFloat1:
            self.__currentState  = qFloat2
            self.shiftAction(input)
        elif self.__currentState == qFloat2:
            self.__currentState = qFloat3
            self.shiftAction(input)
        elif self.__currentState == qFloat3:
            self.__currentState = qFloat4
            self.shiftAction(input)
        elif self.__currentState == qFloat4:
            self.__currentState = qFloat5
            self.shiftAction(input)
        elif self.__currentState == qFloat5:
            self.__currentState = qFloat6
            self.shiftAction(input)
        elif self.__currentState == qFloat6:
            self.__currentState = qFloat7
            self.shiftAction(input)
        elif self.__currentState == qFloat7:
            self.__currentState = qInitial
            self.endFloatAction(input)
        elif self.__currentState == qSymbolIndex0:
            self.__currentState = qSymbolIndex1
            self.shiftAction(input)
        elif self.__currentState == qSymbolIndex1:
            self.__currentState = qSymbolIndex2
            self.shiftAction(input)
        elif self.__currentState == qSymbolIndex2:
            self.__currentState = qSymbolIndex3
            self.shiftAction(input)
        elif self.__currentState == qSymbolIndex3:
            self.__currentState = qInitial
            self.endSymbolIndexAction(input)
        elif self.__currentState == qSymbolCache0:
            self.__currentState = qSymbolCache1
            self.shiftAction(input)
        elif self.__currentState == qSymbolCache1:
            self.__currentState = qSymbolCache2
            self.shiftAction(input)
        elif self.__currentState == qSymbolCache2:
            self.__currentState = qSymbolCache3
            self.shiftAction(input)
        elif self.__currentState == qSymbolCache3:
            self.__currentState = qSymbolCache4
            self.shiftAction(input)
        elif self.__currentState == qSymbolCache4:
            if input == 0:
                self.__currentState = qInitial
                self.endSymbolCacheAction(input)
            else:
                self.bufferShiftAction(input)
        elif self.__currentState == qString:
            if input == 0:
                self.__currentState = qInitial
                self.endStringAction(input)
            else:
                self.bufferShiftAction(input)
        elif self.__currentState == qObject0:
            self.__currentState = qObject1
            self.shiftAction(input)
        elif self.__currentState == qObject1:
            self.__currentState = qObject2
            self.shiftAction(input)
        elif self.__currentState == qObject2:
            self.__currentState = qObject3
            self.shiftAction(input)
        elif self.__currentState == qObject3:
            self.__currentState = qInitial
            self.endObjectAction(input)
    
    def decode(self, data):
        for c in data:
            self.nextState(ord(c))
            if self.__currentState == 0:
                raise FtsClientException

        
        
