# Import os for strerror
import os

class FtsClientException(Exception):
    """Base class for Exception in Client module
    Methods:
        getMessage: return message error
        getErr: return system error id (errno)
    """
    
    # Class constructor
    def __init__(self, message, err_id):
        # Make __messsage and __err_id private
        self.__message = message
        self.__err_id = err_id
        return
    
    def getMessage(self):
        "Return message error"
        return self.__message
    
    def getErr(self):
        "Return system error id (errno)"
        return self.__err_id
    
    # For print call
    def __str__(self):
        to = "FtsClientException: " + self.getMessage() + ' (err="%s")' % os.strerror(self.getErr())
        return to
