def my_hash(s):
    h = 0
    for c in s:
        h = (h << 1) + ord(c)
    return h

class HashTable:
    def __init__(self):
        self.__table = {}

    def get(self, key):
        value = None
        if self.__table.has_key(key):
            status = 1
            value = self.__table[key]
        else:
            status = 0
        return (status, value)
    
    def put(self, key, value):
        self.__table[my_hash(key)] = value
        
