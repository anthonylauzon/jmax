import HashTable

class SymbolCache:
    DEFAULT_INITIAL_CAPACITY = 1031
    
    def __init__(self):
        self.__cache = {}
        return
    
    def index(self, s):
        return HashTable.my_hash(s)
    
    def put(self, index, s):
        self.__cache[index] = s
        return
    
    def get(self, index):
        if self.__cache.has_key(index):
            return self.__cache[index]
        else:
            return None
    
        
    
        
    
