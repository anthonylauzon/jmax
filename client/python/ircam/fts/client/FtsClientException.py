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
