import os
import socket
from FtsClientException import FtsClientException
from FtsServerConnection import FtsServerConnection

class FtsSocketConnection(FtsServerConnection):
    def __init__(self, hostname = "localhost.localdomain", port = 2023):
        try:
            FtsServerConnection.__init__(self)
        except NotImplementedError:
            pass
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((hostname, port))
        self.startThread()
        return
    
    def close(self):
        FtsServerConnection.stopThread(self)
        self.sock.close()
        return
    
    def read(self, byte, length):
        try:
            byte = self.sock.recv(length)
            n  = len(byte)
            return (n, byte)
        except socket.error, err:
            if n < 0:
                raise FtsClientException("Failed to read the input connection", err.args[1], err.args[0])
        if n == 0:
            raise FtsClientException("End of input", 0, "")

    
    def write(self, byte, length):
        try:
            tmp = self.sock.send(byte[:length])
            return
        except socket.error, err:
            raise FtsClientException("Error in sending message", err.args[1], err.args[0])

    
    def __connect(self):
        return
    
    def __connectOnce(self):
        return

    
