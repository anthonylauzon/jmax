#
# jMax
# Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser Lesser General Public License
# as published by the Free Software Foundation; either version 2.1
# of the License, or (at your option) any later version.
# 
# See file COPYING.LIB for further informations on licensing terms.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser Lesser General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 

import os
import socket
import select
from FtsClientException import FtsClientException
from FtsServerConnection import FtsServerConnection

class FtsSocketConnection(FtsServerConnection):
    """
    This class implement a socket based communication with FTS server
    """
    def __init__(self, hostname = "localhost.localdomain", port = 2023):
        """
        Create a socket connected to a running FTS server
        - hostname: hostname of the running FTS server
        - port: port listened by the running FTS server
        """
        try:
            FtsServerConnection.__init__(self)
        except NotImplementedError:
            pass
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((hostname, port))
        self.startThread()
        return
    
    def close(self):
        """
        Close communication with the FTS server
        """
        FtsServerConnection.stopThread(self)
        self.sock.shutdown(2)
        self.sock.close()
        return
    
    def read(self, byte, length):
        """
        Read at most length bytes from the socket connected to the FTS server
        """
        rlist, wlist, xlist = select.select([self.sock],[],[])
        if self.sock in rlist:
            byte = self.sock.recv(length)
            n  = len(byte)
            if n == 0:
                raise FtsClientException("End of input", 0)
            else:
                return (n, byte)
        else:
            # select failed ...., raise exception
            raise FtsClientException("Select failed on read socket, stop listening", 0)
    
    def write(self, byte, length):
        """
        Write length bytes to the socket connected to the FTS server
        """
        tmp = self.sock.send(byte[:length])
        if tmp == 0:
            raise FtsClientException("Error in sending message", 0)
        return

    
    def __connect(self):        
        return
    
    def __connectOnce(self):
        return

    
