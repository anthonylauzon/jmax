class FtsAtom:
    VOID = 1
    INT = 2
    DOUBLE = 3
    SYMBOL = 4
    STRING = 5
    RAW_STRING = 6
    OBJECT = 7
    
    def __init__(self):
        self.__type = FtsAtom.VOID
    
    def isVoid(self):
        return self.__type == FtsAtom.VOID
    
    def isInt(self):
        return self.__type == FtsAtom.INT
    
    def isDouble(self):
        return self.__type == FtsAtom.DOUBLE
    
    def isSymbol(self):
        return self.__type == FtsAtom.SYMBOL
    
    def isString(self):
        return self.__type == FtsAtom.STRING
    
    def isRawString(self):
        return self.__type == FtsAtom.RAW_STRING
    
    def isObject(self):
        return self.__type == FtsAtom.OBJECT
    
    def setVoid(self):
        self.__type = FtsAtom.VOID
    
    def setInt(self, v):
        self.__type = FtsAtom.INT
        self.value = v
    
    def setDouble(self, v):
        self.__type = FtsAtom.INT
        self.value = v
    
    def setSymbol(self, s):
        self.__type = FtsAtom.SYMBOL
        self.value = s
    
    def setString(self, s):
        self.__type = FtsAtom.STRING
        self.value = s
    
    def setRawString(self, s):
        self.__type = FtsAtom.RAW_STRING
        self.value = s
    
    def setObject(self, o):
        self.__type = FtsAtom.OBJECT
        self.value = o
    
    def getType(self):
        return self.__type
    

