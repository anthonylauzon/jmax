from ftsclient import *

# Etablish connection
try:
    connection = FtsSocketConnection()
except FtsClientException:
    print "Receive a FtsClientException"



# Create root patcher
rootPatcher = FtsObject(connection, None, 0)
clientObject = FtsObject(connection, rootPatcher, 1)

console_stream = FtsObject(server, rootPatcher, "console_stream");
console_stream.send("set_default");

