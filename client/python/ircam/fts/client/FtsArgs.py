from FtsAtom import FtsAtom

class FtsArgs:
    def __init__(self):
        self.__current = 0
        tmp = FtsAtom()        
        self.__array = [tmp]
        self.__size = 1
        
    def addInt(self, v):
        self.ensureCapacity(1)
        self.__array[self.__current].setInt(v)
        self.__current += 1
    
    def addDouble(self, v):
        self.ensureCapacity(1)
        self.__array[self.__current].setDouble(v)
        self.__current += 1
    
    def addSymbol(self, v):
        self.ensureCapacity(1)
        self.__array[self.__current].setSymbol(v)
        self.__current += 1
    
    def addString(self, v):
        self.ensureCapacity(1)
        self.__array[self.__current].setString(v)
        self.__current += 1
    
    def addRawString(self, v):
        self.ensureCapacity(1)
        self.__array[self.__current].setRawString(v)
        self.__current += 1
    
    def addObject(self, v):
        self.ensureCapacity(1)
        self.__array[self.__current].setObject(v)
        self.__current += 1
    
    def isInt(self, index):
        return self.__array[index].isInt()
    
    def isDouble(self, index):
        return self.__array[index].isDouble()
    
    def isSymbol(self, index):
        return self.__array[index].isSymbol()
    
    def isString(self, index):
        return self.__array[index].isString()
    
    def isRawString(self, index):
        return self.__array[index].isRawString()
    
    def isObject(self, index):
        return self.__array[index].isObject()
    
    def getInt(self, index):
        return self.__array[index].value
    
    def getDouble(self, index):
        return self.__array[index].value
    
    def getSymbol(self, index):
        return self.__array[index].value
    
    def getString(self, index):
        return self.__array[index].value
    
    def getRawString(self, index):
        return self.__array[index].value
    
    def getObject(self, index):
        return self.__array[index].value
    
    def clear(self):
        self.__current = 0
    
    def getAtoms(self):
        return self.__array
    
    def getLength(self):
        return self.__current
    
    def ensureCapacity(self, wanted):
        if (self.__current + wanted) > self.__size:
            self.__size *= 2
            for i in range(self.__current, self.__size):
                tmp = FtsAtom()
                self.__array.append(tmp)

            
