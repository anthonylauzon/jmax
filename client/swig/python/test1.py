from ftsclient import *

# Etablish connection
try:
    connection = FtsSocketConnection()
except FtsClientException:
    print "Receive a FtsClientException"



# Create root patcher
rootPatcher = FtsObject(connection, None, "jpatcher")

# client object
jMaxClient = FtsObject(connection, rootPatcher, 1)
