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

            
    
