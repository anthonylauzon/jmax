class FtsObject:
    # private static member
    messageHandlersTable = {}
    __lookupEntry = "TO FIXED"
    
    # public:
    # static const int NO_ID = -1
    NO_ID = -1
    NEW_ID = -2
    
    def __init__(self, serverConnection, parent, *args):
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
        from FtsArgs import FtsArgs
        if len(args) > 1:
            print "FtsObject.send want only 2 arguments"
            return
        
        self.__serverConnection.writeObject(self)
        self.__serverConnection.writeSymbol(selector)
        if len(args) == 1:
            if not issubclass(args[0].__class__,FtsArgs):
                print "FtsObject.send want a FtsArgs as last parameters"
                return
            
            self.__serverConnection.writeArgs(args[0])
        self.__serverConnection.endOfMessage()
        return
    
    def getParent(self):
        return self.__parent
    
    def getServerConnection(self):
        return self.__serverConnection
    
    def registerMessageHandler(ftsClass, selector, messageHandler):
        entry = (ftsClass, selector)
        FtsObject.messageHandlersTable[entry] = messageHandler
        return

    registerMessageHandler = staticmethod(registerMessageHandler)
    
    def invokeMessageHandler(obj, selector, args):
        if  selector == obj.selectorCache:
            obj.messageHandlerCache.invoke(obj, args)
            return
        entry = (obj.__class__, selector)

        if FtsObject.messageHandlersTable.has_key(entry):
            messageHandler = FtsObject.messageHandlersTable[entry]
            obj.selectorCache = selector
            obj.messageHandlerCache = messageHandler
            obj.messageHandlerCache.invoke(obj, args)
        
    invokeMessageHandler = staticmethod(invokeMessageHandler)
    
    def getID(self):
        return self.__id
    
    def __setID(self, id):
        self.__id = id
        return


    

