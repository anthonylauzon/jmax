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
import select
from FtsClientException import FtsClientException
from FtsServerConnection import FtsServerConnection

class FtsPipeConnection(FtsServerConnection):
    """
    This class is used for pipe connection between FTS server
    and Python client
    """
    def __init__(self, fts_bin, fts_args):
        """
        Constructor for pipe communication:
        - fts_bin: path to fts executable
        - fts_args: args for fts executable
        """
        try:
            FtsServerConnection.__init__(self)
        except NotImplementedError:
            pass
        [ self.input_stream, self.output_stream, self.err_stream] = os.popen3(fts_bin + " " + fts_args, 'b')
        self.input_fd = self.input_stream.fileno()
        self.output_fd = self.output_stream.fileno()
        self.startThread()
        return

    def close(self):
        """
        Stop reading thread and close the pipe
        """
        FtsServerConnection.stopThread(self)
        self.input_stream.close()
        self.output_stream.close()
        return

    def read(self, byte, length):
        """
        Read at most length bytes from the pipe
        """
        ready = select.select([self.output_fd],[],[], 0.1) # Wait for FTS output
        if self.output_fd in ready[0]:
            # Read byte available in pipe
            byte = os.read(self.output_fd, length)
            # the previous line is not equivalent to
            # byte = self.output_stream.read(length)
            # why ?
            n = len(byte)
        else:
            # pipe is not availabe for reading
            n = 0
            byte = ''
        return (n, byte)

    def write(self, byte, length):
        """
        Write length bytes to the pipe
        """
        self.input_stream.write(byte[:length])
        self.input_stream.flush()
        return

    def __connect(self):
        return

    def __connectOnce(self):
        return

            
    
