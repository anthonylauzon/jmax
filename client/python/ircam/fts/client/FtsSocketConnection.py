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
        return
    
    def read(self, byte, length):
        byte = self.sock.recv(length)
        return (len(byte), byte)
    
    def write(self, byte, length):
        tmp = self.sock.send(byte[:length])
        if tmp < 0:
            raise FtsClientException("Error in sending message", os.errno)
        return
    
    def __connect(self):
        return
    
    def __connectOnce(self):
        return

    
