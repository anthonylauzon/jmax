class Buffer:
    def __init__(self):
        self.__buffer = ""
        return
    
    def clear(self):
        self.__buffer = ""
        return
    
    def append(self, b):
        self.__buffer += b
        return
    
    def getBytes(self):
        return self.__buffer
    
    def getLength(self):
        return len(self.__buffer)

