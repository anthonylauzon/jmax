class FtsObject:
    """
    This class is the base class for all object manipulated by FTS
    """
    # private static member
    __lookupEntry = "TO FIXED"
    
    # public:
    # static const int NO_ID = -1
    NO_ID = -1
    NEW_ID = -2
    
    def __init__(self, serverConnection, parent, *args):
        """
        FtsObject constructor: create an object in server side
        - serverConnection: connection to the FTS server
        - parent: parent object
        - *args can be:
        -- int: object ID to use
        -- int, args[1]: objectID to use, args[1] are send to FTS object constructor
        """
        from FtsServerConnection import FtsServerConnection
        # Common constructor
        self.__serverConnection = serverConnection
        self.__parent = parent
        self.selectorCache = ""
        
        if len(args) > 0:
            if type(args[0]) == int:
                # FtsObject::FtsObject(FtsServerConnection* serverConnection, FtsObject* parent, int id)
                self.__id = args[0]
                if self.__id != FtsObject.NO_ID:
                    self.__serverConnection.putObject(self.__id, self)
                return
            
            self.__id = self.__serverConnection.putObject(FtsObject.NEW_ID, self)
            self.__serverConnection.writeObject(FtsServerConnection.CLIENT_OBJECT_ID)
            self.__serverConnection.writeSymbol("new_object")
            
            self.__serverConnection.writeObject(parent)
            self.__serverConnection.writeInt(self.__id)
            self.__serverConnection.writeSymbol(args[0])
            if len(args) == 1:
                # FtsObject::FtsObject(FtsServerConnection* serverConnection, FtsObject* parent, const char* ftsClassName)
                self.__serverConnection.endOfMessage()
                return
            else:
                #                if type(args[1]) == FtsArgs:
                #                     self.__serverConnection.writeArgs(args[1])
                #                     return
                #                 else:
                #                     print 'Want a FtsArgs for forth argument'
                #                     return
                self.__serverConnection.writeArgs(args[1])
                return
    
    def send(self, selector, *args):
        """
        Send a message with the given selector and args
        """
        if len(args) > 1:
            print "FtsObject.send want only 2 arguments"
            return
        
        self.__serverConnection.writeObject(self)
        self.__serverConnection.writeSymbol(selector)
        if len(args) == 1:
            if not type(args[0])==list:
                print "FtsObject.send want a list as last parameters"
                return
            
            self.__serverConnection.writeArgs(args[0])
        self.__serverConnection.endOfMessage()
        return


    def sendProperty(self,*args) :
        self.__serverConnection.writeObject(self)
        self.__serverConnection.writeSymbol("set_object_property")
        self.__serverConnection.writeObject(self)
        if not type(args[0])==list:
            print "FtsObject.send want a list as last parameters"
            return
        self.__serverConnection.writeArgs(args)
        self.__serverConnection.endOfMessage()
        return


    
    def getParent(self):
        """
        Return object's parent
        """
        return self.__parent
    
    def getServerConnection(self):
        """
        Return object's connection to the FTS server
        """
        return self.__serverConnection
        
    def getID(self):
        """
        Return object ID
        """
        return self.__id
    
    def __setID(self, id):
        """
        Set object ID
        """
        self.__id = id
        return


    

