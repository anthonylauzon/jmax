//
// FTS client library
// Copyright (C) 2001 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

#include <pthread.h>
#include <errno.h>
#include <fts/ftsclient.h>
#include "BinaryProtocolDecoder.h"
#include "BinaryProtocolEncoder.h"
#include "Hashtable.h"

namespace ircam {
namespace fts {
namespace client {

  const int FtsServerConnection::DEFAULT_RECEIVE_BUFFER_SIZE = 65536;
  const int FtsServerConnection::CLIENT_OBJECT_ID = 1;

  void *FtsServerConnection::receiveThread( void *arg)
  {
    FtsServerConnection *connection = reinterpret_cast<FtsServerConnection *>(arg);

  try
    {
      while(1)
	{
	  int n = connection->read( connection->_receiveBuffer, DEFAULT_RECEIVE_BUFFER_SIZE);
  
	  if (n < 0)
	    throw FtsClientException( "Failed to read the input connection");
	  if (n == 0)
	    throw FtsClientException( "End of input");

	  connection->_decoder->decode( connection->_receiveBuffer, n);
	}
    }
  catch( FtsClientException& e)
    {
	std::cerr << " Exception message : " << e.getMessage() << std::endl;
      pthread_exit( 0);
    }

  return 0;
  }
  
  FtsServerConnection::FtsServerConnection() throw( FtsClientException)
  {
    _newObjectID = 16; // Ids 0 to 15 are reserved for pre-defined system objects

    _objectTable = new Hashtable<int, FtsObject*>();

    _decoder = new BinaryProtocolDecoder( this);
    _encoder = new BinaryProtocolEncoder();
    _receiveBuffer = new unsigned char[FtsServerConnection::DEFAULT_RECEIVE_BUFFER_SIZE];
  }

  FtsServerConnection::~FtsServerConnection()
  {
    delete _encoder;
    delete _objectTable;
    /* Stop thread .... */
    delete[] _receiveBuffer;

  }

  FtsObject *FtsServerConnection::getObject( int id)
  {
    FtsObject *obj;
    
    if (_objectTable->get( id, obj))
      return obj;

    return NULL;
  }

  void FtsServerConnection::putObject( int id, FtsObject *obj)
  {
    _objectTable->put( id, obj);
  }

  void FtsServerConnection::writeInt( int v) throw( FtsClientException)
  {
    _encoder->writeInt( v);
  }

  void FtsServerConnection::writeDouble( double v) throw( FtsClientException) 
  {
    _encoder->writeDouble( v); 
  }

  void FtsServerConnection::writeSymbol( const char *v) throw( FtsClientException) 
  {
    _encoder->writeSymbol( v); 
  }

  void FtsServerConnection::writeString( const char *v) throw( FtsClientException) 
  {
    _encoder->writeString( v); 
  }

  void FtsServerConnection::writeRawString( const char *v) throw( FtsClientException) 
  {
    _encoder->writeRawString( v); 
  }

  void FtsServerConnection::writeObject( const FtsObject *v) throw( FtsClientException) 
  {
    _encoder->writeObject( v); 
  }

  /* This version is used for object that have predefined IDs */
  void FtsServerConnection::writeObject( int id) throw( FtsClientException) 
  {
    _encoder->writeObject( id); 
  }

  void FtsServerConnection::writeArgs( const FtsArgs &v) throw( FtsClientException) 
  {
    _encoder->writeArgs( v); 
  }

  void FtsServerConnection::endOfMessage() throw( FtsClientException)
  {
    _encoder->endOfMessage();

    write( _encoder->getBytes(), _encoder->getLength());

    _encoder->clear();
  }

    void FtsServerConnection::startThread() throw(FtsClientException)
    {
	if ( pthread_create( &_receiveThread, NULL, receiveThread, this))
	    throw FtsClientException( "Cannot start receive thread", errno);
	
    }
};
};
};
