//
// FTS client library
// Copyright (C) 2001 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

#include <pthread.h>
#include <errno.h>
#include <fts/client/ftsclient.h>
#include "BinaryProtocolDecoder.h"
#include "BinaryProtocolEncoder.h"
#include "Hashtable.h"

namespace ircam {  
namespace fts {    
namespace client {

  const int FtsServerConnection::DEFAULT_RECEIVE_BUFFER_SIZE = 65536;
  const int FtsServerConnection::CLIENT_OBJECT_ID = 1;
  
#ifdef WIN32
  class ReceiveThread {
  public:
    void start(FtsServerConnection* connection) throw(FtsClientException)
      {
	unsigned long threadID;
	
	_thread = CreateThread(NULL, 0, run, (LPVOID) connection, 0, &threadID);
	
	if (_thread == NULL) 
	  throw FtsClientException( "Cannot start receive thread");
	
	SetThreadPriority(_thread, THREAD_PRIORITY_HIGHEST);
      }
    
    void stop()
      {
      }
    
    static DWORD WINAPI run(LPVOID arg)
      {
	FtsServerConnection *connection = reinterpret_cast<FtsServerConnection *>(arg);	
	connection->receiveLoop();
      }
    
  private:
    HANDLE _thread;
  };
#else
  class ReceiveThread {
  public:
    
    void start(FtsServerConnection* connection) throw(FtsClientException)
      {
	if ( pthread_create( &_thread, NULL, run, static_cast<void*>(connection)))
	  throw FtsClientException( "Cannot start receive thread", errno);
      }
    
    void stop() 
      {
	pthread_cancel(_thread);
      }
    
    static void *run( void *arg)
      {
	FtsServerConnection *connection = reinterpret_cast<FtsServerConnection *>(arg);	
	connection->receiveLoop();	
	pthread_exit( 0);
      }
    
  private:
    pthread_t _thread;
  };
#endif
  
  void FtsServerConnection::receiveLoop()
  {
    try
    {
      while(1)
      {
	int n = read( _receiveBuffer, DEFAULT_RECEIVE_BUFFER_SIZE);
	
	if (n < 0)
	  throw FtsClientException( "Failed to read the input connection");
	if (n == 0)
	  throw FtsClientException( "End of input");

	_decoder->decode( _receiveBuffer, n);
      }
    }
    catch( FtsClientException& e)
    {
      std::cerr << " Exception message : " << e.getMessage() << std::endl;
    }
  }
  
  FtsServerConnection::FtsServerConnection() throw( FtsClientException)
  {
    _newObjectID = 16; // Ids 0 to 15 are reserved for pre-defined system objects
    
    _objectTable = new Hashtable<int, FtsObject*>();    
    _decoder = new BinaryProtocolDecoder( this);
    _encoder = new BinaryProtocolEncoder();
    _receiveBuffer = new unsigned char[FtsServerConnection::DEFAULT_RECEIVE_BUFFER_SIZE];
    _receiveThread = new ReceiveThread();
  }
  
  FtsServerConnection::~FtsServerConnection()
  {
    delete _objectTable;
    delete _decoder;
    delete _encoder;	
    /* Stop thread .... */
    _receiveThread->stop();
    delete _receiveThread;
    delete[] _receiveBuffer;	
  }
  
  FtsObject *FtsServerConnection::getObject( int id)
  {
    FtsObject *obj;
    
    if (_objectTable->get( id, obj))
      return obj;
    
    return NULL;
  }
  
  int FtsServerConnection::putObject( int id, FtsObject *obj)
  {
    if (id == FtsObject::NO_ID)
      return FtsObject::NO_ID;
    
    if (id == FtsObject::NEW_ID)
      id = getNewObjectID();
    
    _objectTable->put( id, obj);
    
    return id;
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
    _receiveThread->start( this);	
  }
}; // namespace client
}; // namespace fts
}; // namespace ircam

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset:2
 * End:
 */
