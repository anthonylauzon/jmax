/*
 * FTS client library
 * Copyright (C) 2001 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#ifndef WIN32
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>
#define LASTERROR errno
#else
#define LASTERROR WSAGetLastError()
#define snprintf _snprintf
#define sleep Sleep
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <fts/ftsclient.h>


/***************************************************
 * ftsclient log
 *
 * Should be rewritten and integrated in the existing classes
 */

static char* log_file = NULL;
static unsigned int log_start;

char* ftsclient_curdate(char* buf, int len)
{
#ifdef WIN32
  char t[9];
  char d[9];
  _strdate(d);
  _strtime(t);
  snprintf(buf, len, "%s %s", d, t);
#else
  time_t t;

  time( &t);
  snprintf(buf, len, "%s", ctime( &t));

#endif
  return buf;
}

unsigned int ftsclient_curtime()
{
#ifdef WIN32
  return GetTickCount();
#else
  struct timeval now;
  gettimeofday(&now, NULL);
  return now.tv_sec * 1000 + now.tv_usec / 1000;
#endif
}

void ftsclient_init_log(void)
{
  FILE* log;
  char date[64];

#ifdef WIN32
  log_file = "C:\\ftsclient_log.txt";
#else
  char* home = getenv("HOME");
  struct timeval now;
  if (home) {
    char buf[1024];
    snprintf(buf, 1024, "%s/.ftsclient_log", home);
    log_file = strdup(buf);
  } else {
    log_file = "/tmp/ftsclient_log";
  }
#endif

  log_start = ftsclient_curtime();

  /* truncate the file */
  log = fopen(log_file, "w");
  fprintf(log, "[log]: ftsclient compiled on %s at %s\n", __DATE__, __TIME__);
  fprintf(log, "[log]: %s started logging\n", ftsclient_curdate(date, 64));
  fclose(log);
}

void ftsclient_log(char* fmt, ...)
{
  FILE* log;
  va_list args; 

  if (log_file == NULL) {
    ftsclient_init_log();
  }

  log = fopen(log_file, "a");
  if (log == NULL) {
    return;
  }

  fprintf(log, "[%d]", ftsclient_curtime() - log_start); 

  va_start (args, fmt); 
  vfprintf(log, fmt, args); 
  va_end (args); 

  fflush(log);
  fclose(log);
}

/***************************************************
 * FtsHashTable
 */

static const unsigned int primesSuite[] = {
  7,
  17,
  31,
  67,
  127,
  257,
  521,
  1031,
  2053,
  4099,
  8191,
  16411,
  32771,
  65537,
  131071,
  262147,
  524287,
  1048583,
  2097169,
  4194319,
  8388617,
  16777259,
  33554467,
};

unsigned int getNextPrime( unsigned int n)
{
  unsigned int i;

  for ( i = 0; i < sizeof (primesSuite) / sizeof (unsigned int); i++)
    if (n < primesSuite[i])
      return primesSuite[i];

  return primesSuite[i-1];
}


/***************************************************
 * FtsClientException
 */

ostream &FtsClientException::print( ostream &os) const
{
  if (_err != 0)
    os << "FtsClientException: " << _message << " (err=\"" << strerror( _err) << "\")";
  else
    os << "FtsClientException: " << _message;

  return os;
}

ostream &operator<<( ostream &os, const FtsClientException &e)
{
  return e.print( os);
}


/***************************************************
 * FtsValue
 */

const int FtsValue::EMPTY = -1;
const int FtsValue::INT = 1;
const int FtsValue::FLOAT = 2;
const int FtsValue::STRING = 3;
const int FtsValue::OBJECT = 4;

/***************************************************
 * FtsArgs
 */

void FtsArgs::clear()
{
  for( int i = 0; i < length(); i++)
    _buffer[i].unset();

  _buffer.clear();
}

FtsArgs::FtsArgs( const FtsArgs &args)
{
  FtsArgs &a = (FtsArgs &)args;

  for ( int i = 0; i < a.length(); i++)
    {
      if (a.isInt(i))
	add( a.getInt(i));
      else if (a.isFloat(i))
	add( a.getFloat(i));
      else if (a.isString(i))
	add( a.getString(i));
      else if (a.isObject(i))
	add( a.getObject(i));
    }
}

ostream &FtsArgs::print( ostream &os)
{
  os << "[ ";
  for ( int i = 0; i < length(); i++)
    {
      if ( isInt( i))
	os << getInt( i);
      else if ( isFloat( i))
	os << getFloat( i);
      else if ( isString( i))
	os << getString( i);
      else if ( isObject( i))
	os << getObject( i);

      os << " ";
    }

  os << " ]";

  return os;
}

ostream &operator<<( ostream &os, FtsArgs &args)
{
  return args.print( os);
}

/* ********************************************************************** */
/* FtsServerConnnection and derived classes                               */
/* ********************************************************************** */
const int FtsSocketConnection::DEFAULT_PORT = 2023;
const int FtsSocketConnection::DEFAULT_CONNECT_TIMEOUT = 30;

int FtsSocketConnection::_initializedSocketLayer = 0;

void FtsSocketConnection::initializeSocketLayer()
{
#if defined(WIN32)
  if (_initializedSocketLayer) {
    return;
  }

  _initializedSocketLayer++;

  WORD wVersionRequested;
  WSADATA wsaData;
  int result;

  wVersionRequested = MAKEWORD(2, 2);
  
  result = WSAStartup( wVersionRequested, &wsaData );
  if (result != 0) {
    throw FtsClientException( "Couldn't initialize WinSock", result);
  }
  
  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
    WSACleanup();
    throw FtsClientException( "Bad WinSock version");
  }
#endif
}

int FtsSocketConnection::connectOnce() throw( FtsClientException )
{
  struct sockaddr_in server_addr;
  struct hostent *hostptr;
  struct in_addr addr;

  /* convert the host name to the inet address. call gethostbyname
     only when the host is not in a numbers-and-dots notation. this
     avoids a name resolution on windows machines */
  addr.s_addr = inet_addr( _hostname);

  if (addr.s_addr == INADDR_NONE) {

    /* host is not a numbers-and-dots notation. resolve the name. */
    hostptr = gethostbyname( _hostname);

    if ( !hostptr)
      throw FtsClientException( "Unknown host");

    addr = *(struct in_addr *)hostptr->h_addr_list[0];
  }

  _socket = socket( PF_INET, SOCK_STREAM, 0);

  if (_socket == INVALID_SOCKET)
    throw FtsClientException( "Can't create socket", LASTERROR);

  memset( &server_addr, 0, sizeof( server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr = addr;
  server_addr.sin_port = htons( _port);

  if (::connect( _socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0)
    return 0;

  CLOSESOCKET( _socket);

  return -1;
}

void FtsSocketConnection::connect() throw( FtsClientException )
{
  int r;

  do
    {
      if ((r = connectOnce()) == 0)
	break;

      sleep( 1);
      _connectTimeout--;
    }
  while (_connectTimeout > 0);

  if ( r < 0)
    throw FtsClientException( "Cannot connect", errno);
}

FtsSocketConnection::FtsSocketConnection( const char *hostname, int port, int connectTimeout) throw( FtsClientException)
  : _socket( INVALID_SOCKET), _hostname( hostname), _port( port), _connectTimeout( connectTimeout)
{
  initializeSocketLayer();
  connect();
}

FtsSocketConnection::FtsSocketConnection() throw( FtsClientException)
  : _socket( INVALID_SOCKET), _hostname( "127.0.0.1"), _port( DEFAULT_PORT), _connectTimeout( DEFAULT_CONNECT_TIMEOUT)
{
  initializeSocketLayer();
  connect();
}

int FtsSocketConnection::poll() throw( FtsClientException)
{
  fd_set readfds;
  struct timeval tv;
  int r;

  FD_ZERO( &readfds);
  FD_SET( _socket, &readfds);

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  r = select( _socket+1, &readfds, NULL, NULL, &tv);

  if (r < 0)
    throw FtsClientException( "Error in select()", LASTERROR);

  return r;
}

void FtsSocketConnection::close() throw( FtsClientException)
{
#ifdef WIN32
  int r;
  char buf[1024];
  if (_socket != INVALID_SOCKET) {
    /* call WSAAsyncSelect ??? */
    ::shutdown(_socket, 0x02);
    while (1) {
      r = ::recv(_socket, buf, 1024, 0);
      if ((r == 0) || (r == SOCKET_ERROR)) {
	break;
      }
    }
  }
#endif

  if (_socket != INVALID_SOCKET) {
    CLOSESOCKET(_socket);
  }
}

int FtsSocketConnection::read( unsigned char *buffer, int n) throw( FtsClientException)
{
  int r;

  r = SOCKETREAD( _socket, buffer, n);
  if (r == SOCKET_ERROR)
    {
      throw FtsClientException( "Error in message receiving", LASTERROR);
    }
  if (r == 0)
    {
      throw FtsClientException( "Socket closed");
    }

  return r;
}

int FtsSocketConnection::write( const unsigned char *buffer, int n) throw( FtsClientException)
{
  int r;

  if ( (r = SOCKETWRITE( _socket, buffer, n)) < 0)
    {
      throw FtsClientException( "Error in sending message", LASTERROR);
    }

  return r;
}


/* ********************************************************************** */
/* FtsPipeConnection                                                      */
/* ********************************************************************** */

FtsPipeConnection::FtsPipeConnection( FtsProcess *fts)
{
  _in = fts->getInputPipe();
  _out = fts->getOutputPipe();
}

FtsPipeConnection::~FtsPipeConnection(void)
{
  close();
}

void FtsPipeConnection::close() throw( FtsClientException)
{
  // Don't do anything. We didn't create the pipes, so we don't
  // destroy them.
}

int FtsPipeConnection::read( unsigned char *buffer, int n) throw( FtsClientException) 
{
#if WIN32
  DWORD count; 

  if (!ReadFile(_in, (LPVOID) buffer, (DWORD) n, &count, NULL)) {

    /* keep a trace of the error in the log file */
    LPVOID msg;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &msg, 0, NULL);
    ftsclient_log("[pipe]: failed to write to pipe: (%s)\n", msg);
    LocalFree(msg);

    /* return -1 and let the caller handle the error situation */
    return -1;
  }

  return count;

#else
  int r;

  if ( (r = ::read( _in, buffer, n)) < 0)
    throw FtsClientException( "Error in sending message", LASTERROR);

  return r;
#endif
}

int FtsPipeConnection::write( const unsigned char *buffer, int n) throw( FtsClientException)
{
#if WIN32
  DWORD count; 

  if (!WriteFile(_out, (LPCVOID) buffer, (DWORD) n, &count, NULL)) {

    /* keep a trace of the error in the log file */
    LPVOID msg;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &msg, 0, NULL);
    ftsclient_log("[pipe]: failed to write to pipe: (%s)\n", msg);
    LocalFree(msg);

    /* return -1 and let the caller handle the error situation */
    return -1;
  }

  return count;

#else
  int r;

  if ( (r = ::write( _out, buffer, n)) < 0)
    throw FtsClientException( "Error in sending message", LASTERROR);

  return r;
#endif
}

int FtsPipeConnection::poll() throw( FtsClientException)
{
#if WIN32
  DWORD available = 0;

  if (PeekNamedPipe(_in, NULL, 0, NULL, &available, NULL)) {
    return available > 0;
  } else {
    return 0;
  } 
 
#else
  fd_set readfds;
  struct timeval tv;
  int r;

  FD_ZERO( &readfds);
  FD_SET( _in, &readfds);

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  r = select( _in+1, &readfds, NULL, NULL, &tv);

  if (r < 0)
    throw FtsClientException( "Error in select()", LASTERROR);

  return r;
#endif
}

/* ********************************************************************** */
/* FtsServer                                                              */
/* ********************************************************************** */

const int FtsServer::DEFAULT_RECEIVE_BUFFER_SIZE = 65536;

FtsServer::FtsServer( FtsServerConnection *connection, int threaded)
{
  _connection = connection;

  _object = 0;
  _remote = 0;
  _root = 0;
  _args = new FtsArgs();

  _newObjectId = 16; // Ids 0 to 15 are reserved for pre-defined system objects
  _receiveBuffer = new unsigned char[DEFAULT_RECEIVE_BUFFER_SIZE];

  _receiveThread = (thread_t)NULL;
  _serverState = NOT_CONNECTED;

  // Create objects with predefined IDs
  _root = new FtsObject( this, 0);
  _remote = new FtsObject( this, 1);

  _state = 0;

  _serverState = CONNECTED;

  if (!threaded)
    return;

#ifdef WIN32
  unsigned long threadID;

  _receiveThread = CreateThread(NULL, 0, receiveThread, (LPVOID) this, 0, &threadID);
  
  if (_receiveThread == NULL) 
    throw FtsClientException( "Cannot start receive thread");
  
  SetThreadPriority(_receiveThread, THREAD_PRIORITY_HIGHEST);
  
#else
  
  if ( pthread_create( &_receiveThread, NULL, receiveThread, this))
    throw FtsClientException( "Cannot start receive thread", errno);

  signal( SIGPIPE, SIG_IGN);
#endif
}

FtsServer::~FtsServer()
{
  if (_serverState == CONNECTED)
    disconnect();

  if (_receiveBuffer) 
    delete _receiveBuffer;

  if (_remote)
    delete _remote;

  if (_root)
    delete _root;

  if (_args)
    delete _args;
}

void FtsServer::disconnect() throw( FtsClientException)
{
  if (_receiveThread)
    wait();

  _serverState = DISCONNECTED;
}

void FtsServer::shutdown() throw( FtsClientException)
{
  _remote->send( "shutdown");
}

#if defined(WIN32)
DWORD WINAPI FtsServer::receiveThread(LPVOID arg)
#else
void *FtsServer::receiveThread( void *arg)
#endif
{
  FtsServer *server = (FtsServer *)arg;

  try
    {
      for (;;) 
	server->receive();
    }
  catch( FtsClientException e)
    {
      ftsclient_log("[receive]: %s\n", e.getMessage());
#if defined(WIN32)
      return 0;
#else
      pthread_exit( 0);
#endif
    }

  return 0;
}

void FtsServer::receive() throw( FtsClientException)
{
  int n;

  n = _connection->read(_receiveBuffer, DEFAULT_RECEIVE_BUFFER_SIZE);
  
  if (n < 0) {
    throw FtsClientException( "Failed to read the input connection");
  }
  if (n == 0) {
    throw FtsClientException( "End of input");
  }

  decode( _receiveBuffer, n);
}

void FtsServer::poll() throw( FtsClientException)
{
  if (_connection->poll())
    receive();
}

void FtsServer::wait() throw( FtsClientException)
{
#ifdef WIN32
  WaitForSingleObject(_receiveThread, INFINITE);
#else
  if (pthread_join( _receiveThread, NULL))
    throw FtsClientException( "Cannot join receive thread", errno);
#endif
}


FtsObject *FtsServer::getObject( int id)
{
  FtsObject *object = 0;

  _objectTable.get( id, object);
  return object;
}


void FtsServer::startMessage()
{
  _message.clear();
}

void FtsServer::put( int n)
{
  *_message = (char) ((n >> 24) & 0xff);
  _message++;
  *_message = (char) ((n >> 16) & 0xff);
  _message++;
  *_message = (char) ((n >> 8) & 0xff);
  _message++;
  *_message = (char) ((n >> 0) & 0xff);
  _message++;
}

void FtsServer::encode( int n)
{
  *_message = FTS_PROTOCOL_INT;
  _message++;
  put( n);
}

void FtsServer::encode( float f)
{
  *_message = FTS_PROTOCOL_FLOAT;
  _message++;
  put( *((int *)&f) );
}

void FtsServer::encode( const char *s)
{
  *_message = FTS_PROTOCOL_STRING;
  _message++;

  while (*s)
    {
      *_message = *s++;
      _message++;
    }
  
  *_message = FTS_PROTOCOL_STRING_END;
  _message++;
}

void FtsServer::encode( const FtsObject *o)
{
  *_message = FTS_PROTOCOL_OBJECT;
  _message++;

  if (o)
    put( o->getID() );
  else
    put( 0);
}

void FtsServer::encode( FtsArgs &args)
{
  for ( int i = 0; i < args.length(); i++)
    {
      if ( args.isInt( i))
	encode( args.getInt( i));
      else if ( args.isFloat( i))
	encode( args.getFloat( i));
      else if ( args.isString( i))
	encode( args.getString( i));
      else if ( args.isObject( i))
	encode( args.getObject( i));
    }
}

void FtsServer::endMessage() throw( FtsClientException)
{
  *_message = FTS_PROTOCOL_END_OF_MESSAGE;
  _message++;

  if (_serverState != CONNECTED)
    throw FtsClientException( "Error in sending message");

  _connection->write( _message, _message.length());
}

void FtsServer::aEndObject()
{
  _ival = _ival << 8 | _incoming;

  _object = 0;
  _objectTable.get( _ival, _object);
}

void FtsServer::aEndSelector()
{
  *_selector = '\0';
  _args->clear();
}

void FtsServer::aEndIntArg()
{
  _args->add( _ival << 8 | _incoming);
} 

void FtsServer::aEndFloatArg()
{
  _ival = _ival << 8 | _incoming;

  _args->add( *((float *)&_ival));
}

void FtsServer::aEndStringArg()
{
  *_buff = '\0';
  _buff++;

  _args->add( _buff);
}

void FtsServer::aEndObjectArg()
{
  _ival = _ival << 8 | _incoming;

  FtsObject *object = 0;
  if (_objectTable.get( _ival, object))
    _args->add( object);
}

void FtsServer::aEndMessage()
{
  FtsCallback *callback = 0;

  if (!_object) {
    throw FtsClientException( "Message to non-existent object");
  }

  if (!_object->_callbacks) {
    return;
  }

  ftsclient_log("[receive]: message \"%s\" for object[id=%d]\n", (const char*)_selector, _object->getID());

  _object->_callbacks->get( _selector, callback);

  if (callback)
    {
      callback->invoke( _args);
    }
  else
    {
      _object->_callbacks->get( "*", callback);

      if (callback)
	{
	  callback->invoke( _selector, _args);
	} 
      else
	{
	  ftsclient_log("[receive]: no callback found\n");
	}
    }
}

void FtsServer::decode( unsigned char *buffer, int size) throw (FtsClientException)
{

  for ( int i = 0; i < size; i++)
    {
      _incoming = buffer[i];

#define MOVE( VALUE, STATE, ACTION) if ( _incoming==(VALUE)) { _state = STATE; ACTION; break; }
#define UMOVE( STATE, ACTION) _state = STATE; ACTION; break;

      switch( _state) {
      case 0:
	MOVE( FTS_PROTOCOL_OBJECT, 1, _ival = 0);
	UMOVE( 0, throw FtsClientException( "Error in protocol decoding"));
      case 1:
	UMOVE( 2, _ival = _ival << 8 | _incoming);
      case 2:
	UMOVE( 3, _ival = _ival << 8 | _incoming);
      case 3:
	UMOVE( 4, _ival = _ival << 8 | _incoming);
      case 4:
	UMOVE( 5, aEndObject() );
      case 5:
	MOVE( FTS_PROTOCOL_STRING, 6, _selector.clear());
	UMOVE( 0, throw FtsClientException( "Error in protocol decoding"));
      case 6:
	MOVE( FTS_PROTOCOL_STRING_END, 10, aEndSelector() );
	UMOVE( 6, (*_selector = _incoming, _selector++) );
      case 10:
	MOVE( FTS_PROTOCOL_INT, 20, _ival = 0);
	MOVE( FTS_PROTOCOL_FLOAT, 30, _ival = 0);
	MOVE( FTS_PROTOCOL_STRING, 40, _buff.clear());
	MOVE( FTS_PROTOCOL_OBJECT, 50, _ival = 0);
	MOVE( FTS_PROTOCOL_END_OF_MESSAGE, 0, aEndMessage() );
	UMOVE( 0, throw FtsClientException( "Error in protocol decoding"));
      case 20:
	UMOVE( 21, _ival = _ival << 8 | _incoming);
      case 21:
	UMOVE( 22, _ival = _ival << 8 | _incoming);
      case 22:
	UMOVE( 23, _ival = _ival << 8 | _incoming);
      case 23:
	UMOVE( 10, aEndIntArg());
      case 30:
	UMOVE( 31, _ival = _ival << 8 | _incoming);
	break;
      case 31:
	UMOVE( 32, _ival = _ival << 8 | _incoming);
	break;
      case 32:
	UMOVE( 33, _ival = _ival << 8 | _incoming);
	break;
      case 33:
	UMOVE( 10, aEndFloatArg() );
	break;
      case 40:
	MOVE( FTS_PROTOCOL_STRING_END, 10, aEndStringArg() );
	UMOVE( 40, (*_buff = _incoming, _buff++) );
      case 50:
	UMOVE( 51, _ival = _ival << 8 | _incoming);
      case 51:
	UMOVE( 52, _ival = _ival << 8 | _incoming);
      case 52:
	UMOVE( 53, _ival = _ival << 8 | _incoming);
      case 53:
	UMOVE( 10, aEndObjectArg() );
      }
    }
}

/* ********************************************************************** */
/* FtsObject                                                              */
/* ********************************************************************** */

FtsObject::FtsObject( FtsServer *server, FtsObject *parent, const char *ftsClassName) throw( FtsClientException)
  : _server( server), _callbacks( 0)
{
  _id = _server->_newObjectId++;
  _server->_objectTable.put( _id, this);

  // Send a "new_object" message
  _server->startMessage();
  _server->encode( _server->_remote);
  _server->encode( "new_object");
  _server->encode( parent);
  _server->encode( _id);
  _server->encode( ftsClassName);
  _server->endMessage();
}

FtsObject::FtsObject( FtsServer *server, FtsObject *parent, const char *ftsClassName, FtsArgs &args) throw( FtsClientException)
  : _server( server), _callbacks( 0)
{
  _id = _server->_newObjectId++;
  _server->_objectTable.put( _id, this);

  // Send a "new_object" message
  _server->startMessage();
  _server->encode( _server->_remote);
  _server->encode( "new_object");
  _server->encode( parent);
  _server->encode( _id);
  _server->encode( ftsClassName);
  _server->encode( args);
  _server->endMessage();
}

FtsObject::FtsObject( FtsServer *server, int id)
  : _server( server), _id( id), _callbacks( 0)
{
  _server->_objectTable.put( _id, this);
}

FtsObject::~FtsObject() throw( FtsClientException)
{
  _server->_objectTable.remove( _id);

  if (_callbacks)
    delete _callbacks;
}

void FtsObject::destroy() throw( FtsClientException)
{
  // Send a "delete_object" message
  _server->startMessage();
  _server->encode( _server->_remote);
  _server->encode( "delete_object");
  _server->encode( this);
  _server->endMessage();
}

void FtsObject::connectTo( int outlet, FtsObject *dst, int dstInlet)
{
  _server->startMessage();
  _server->encode( _server->_remote);
  _server->encode( "connect_object");
  _server->encode( this);
  _server->encode( outlet);
  _server->encode( dst);
  _server->encode( dstInlet);
  _server->endMessage();
}

void FtsObject::connectFrom( int inlet, FtsObject *src, int srcOutlet)
{
  _server->startMessage();
  _server->encode( _server->_remote);
  _server->encode( "connect_object");
  _server->encode( src);
  _server->encode( srcOutlet);
  _server->encode( this);
  _server->encode( inlet);
  _server->endMessage();
}

void FtsObject::send( const char *selector, FtsArgs &args) throw( FtsClientException)
{
  _server->startMessage();
  _server->encode( this);
  _server->encode( selector);
  _server->encode( args);
  _server->endMessage();
}

void FtsObject::send( const char *selector) throw( FtsClientException)
{
  _server->startMessage();
  _server->encode( this);
  _server->encode( selector);
  _server->endMessage();
}

void FtsObject::send( FtsArgs &args) throw( FtsClientException)
{
  _server->startMessage();
  _server->encode( this);
  _server->encode( "list");
  _server->encode( args);
  _server->endMessage();
}

void FtsObject::send( int n) throw( FtsClientException)
{
  _server->startMessage();
  _server->encode( this);
  _server->encode( "int");
  _server->encode( n);
  _server->endMessage();
}

void FtsObject::send( float f) throw( FtsClientException)
{
  _server->startMessage();
  _server->encode( this);
  _server->encode( "float");
  _server->encode( f);
  _server->endMessage();
}

void FtsObject::install( const char *s, FtsCallback *callback)
{
  if ( !_callbacks)
    _callbacks = new FtsHashTable< const char *, FtsCallback *>( 11);

  _callbacks->put( s, callback);
}

void FtsObject::install( FtsCallback *callback)
{
  if ( !_callbacks)
    _callbacks = new FtsHashTable< const char *, FtsCallback *>( 11);

  _callbacks->put( "*", callback);
}

/***************************************************
 * FtsProcess
 */

#if WIN32
void FtsProcess::init( const char *path, FtsArgs &args) throw( FtsClientException)
{
  BOOL result;
  char cmdLine[2048];
  PROCESS_INFORMATION process_info;
  STARTUPINFO startup_info;
  pipe_t new_stdin, new_stdout, tmp_in, tmp_out; 
  SECURITY_ATTRIBUTES attr; 
  int i;

  _path = (path)? strdup(path) : 0;
  _in = INVALID_PIPE;
  _out = INVALID_PIPE;

  if (_path == 0) {
    findDefaultPath();
  }

  ftsclient_log("[ftsxtra]: debug \n", _path);
  ftsclient_log("[ftsxtra]: path %s\n", _path);

  cmdLine[0] = 0;
  strcat(cmdLine, _path);
  strcat(cmdLine, " ");
  for (i = 0; i < args.length(); i++) {
    if ( args.isString(i))
      {
	strcat( cmdLine, args.getString( i));
	strcat(cmdLine, " ");
      }
  }

  /* 
     create two pipes and set the stdin and stdout of FTS to the pipes 
  */

  /* make sure that the pipe handles can be inherited by FTS */
  attr.nLength = sizeof(SECURITY_ATTRIBUTES); 
  attr.bInheritHandle = TRUE; 
  attr.lpSecurityDescriptor = NULL; 

  /* keep a handle to the old stdout */
  pipe_t old_stdout = GetStdHandle(STD_OUTPUT_HANDLE); 

  /* create a pipe for the new stdout */
  if (!CreatePipe(&tmp_in, &new_stdout, &attr, 0)) {
    throw FtsClientException("Can't create the anonymous pipe for output");
  }

  /* set the pipe as the new stdout so FTS can access it */
  if (!SetStdHandle(STD_OUTPUT_HANDLE, new_stdout)) {
    throw FtsClientException("Can't set the anonymous pipe as output");
  }

#if 0
  if (!DuplicateHandle(GetCurrentProcess(), &tmp_in,
		       GetCurrentProcess(), &this->_in, 
		       0, FALSE, DUPLICATE_SAME_ACCESS)) {
    throw FtsClientException("Failed to duplicate the stdin handle");
  }
  CloseHandle(tmp_in);
#else
  this->_in = tmp_in;
#endif

  /* keep a handle to the old stdin */
  pipe_t old_stdin = GetStdHandle(STD_INPUT_HANDLE); 

  /* create a pipe for the new stdin */
  if (!CreatePipe(&new_stdin, &tmp_out, &attr, 0)) {
    throw FtsClientException("Can't create the anonymous pipe for input");
  }

  /* set the pipe as the new stdin so FTS can access it */
  if (!SetStdHandle(STD_INPUT_HANDLE, new_stdin)) {
    throw FtsClientException("Can't set the anonymous pipe as input");
  }

  if (!DuplicateHandle(GetCurrentProcess(), tmp_out, 
		       GetCurrentProcess(), &this->_out, 
		       0, FALSE, DUPLICATE_SAME_ACCESS)) {
    throw FtsClientException("Failed to duplicate the stdout handle");
  }
 
   CloseHandle(tmp_out); 



  /* 
     start FTS
  */

  ZeroMemory(&startup_info, sizeof(STARTUPINFO));
  startup_info.cb = sizeof(STARTUPINFO); 

  result = CreateProcess(_path, cmdLine, NULL, NULL, TRUE, REALTIME_PRIORITY_CLASS, 
			 NULL, NULL, &startup_info, &process_info);
  if (result == 0) {
    throw FtsClientException("Failed to start the fts application\n");    
  }

  if (!SetStdHandle(STD_INPUT_HANDLE, old_stdin)) { 
    throw FtsClientException("Failed to set the restore the stdin\n");    
  }
  
  if (!SetStdHandle(STD_OUTPUT_HANDLE, old_stdout)) {
    throw FtsClientException("Failed to set the restore the stdout\n");    
  }

}
#else
void FtsProcess::init( const char *path, FtsArgs &args) throw( FtsClientException)
{
  int from_fts_pipe[2];
  int to_fts_pipe[2];

  _path = (path)? strdup(path) : 0;
  _in = INVALID_PIPE;
  _out = INVALID_PIPE;

  if (_path == 0)
    findDefaultPath();

  if ( pipe( from_fts_pipe) < 0)
    throw FtsClientException( "Can't open pipe", errno);

  if ( pipe( to_fts_pipe) < 0)
    throw FtsClientException( "Can't open pipe", errno);

  if ( (_childPid = fork()) < 0)
    {
      throw FtsClientException( "Can't fork FTS", errno);
    }
  else if ( !_childPid)
    {
      char **argv;

      /* FTS standard input is to_fts_pipe */
      close( to_fts_pipe[1]);
      if ( dup2( to_fts_pipe[0], 0) < 0)
	throw FtsClientException( "dup2() failed");
      close( to_fts_pipe[0]);

      /* FTS standard output is from_fts_pipe */
      close( from_fts_pipe[0]);
      if ( dup2( from_fts_pipe[1], 1) < 0)
	throw FtsClientException( "dup2() failed");
      close( from_fts_pipe[1]);

      int i;

      argv = new char *[args.length()+2];

      argv[0] = (char *)_path;

      for ( i = 0; i < args.length(); i++)
	if ( args.isString(i))
	  argv[i+1] = (char *)args.getString( i);

      argv[i+1] = NULL;

      if ( execvp( _path, argv) < 0)
	throw FtsClientException( "execvp() failed", errno);

      exit( 1);
    }
  else
    {
      close( from_fts_pipe[1]);
      _in = from_fts_pipe[0];
      close( to_fts_pipe[0]);
      _out = to_fts_pipe[1];
    }
}
#endif


#if WIN32
void FtsProcess::findDefaultPath() throw( FtsClientException)
{
  unsigned char buf[1024];
  unsigned char version[256];
  char fullpath[1024];
  HKEY key, version_key;
  DWORD type, size;
  char* jmax_key = "Software\\Ircam\\jMax";
  char* jmax_version = "FtsVersion";
  char* jmax_root = "ftsRoot";

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, jmax_key, 0, KEY_READ, &key) != 0) {
    throw FtsClientException("Failed to open the jMax registry key\n");
  }

  if ((RegQueryValueEx(key, jmax_version, 0, &type, 0, &size) != 0)
      || (type != REG_SZ)
      || (size >= 256)
      || (RegQueryValueEx(key, jmax_version, 0, 0, &version[0], &size) != 0)) {
    RegCloseKey(key);
    throw FtsClientException("Failed to read the jMax version registry key\n");
  }

  if (RegOpenKeyEx(key, (const char*) &version[0], 0, KEY_READ, &version_key) != 0) {
    RegCloseKey(key);
    throw FtsClientException("Error opening registry key\n");
  }

  if ((RegQueryValueEx(version_key, jmax_root, 0, &type, 0, &size) != 0)
      || (type != REG_SZ)
      || (size >= 1024)
      || (RegQueryValueEx(version_key, jmax_root, 0, 0, &buf[0], &size) != 0)) {
    RegCloseKey(key);
    RegCloseKey(version_key);
    throw FtsClientException("Failed to read the jmaxRoot registry key\n");
  }

  RegCloseKey(key);
  RegCloseKey(version_key);
  
  snprintf(fullpath, 1024, "%s\\bin\\%s", buf, "fts.exe");
  _path = strdup(fullpath);
}
#else
void FtsProcess::findDefaultPath() throw( FtsClientException)
{
  // We hope that the executable will be in PATH, as we use execvp
  _path = "fts";
}
#endif

/***************************************************
 * FtsPlugin
 */

#ifdef WIN32
#define FTS_LIBRARY      "fts.dll"
#else
#define FTS_LIBRARY      "libfts.so"
#endif

void FtsPlugin::init( FtsArgs &args) throw( FtsClientException)
{
#ifdef WIN32
  unsigned long threadID;
#endif
  int i;

  _thread = (thread_t)NULL;
  _library = NULL;
  _init_function = NULL;
  _run_function = NULL;
  _halt_function = NULL;
  _argc = 0;
  _argv = NULL;

  _library = openLibrary(FTS_LIBRARY);
  if (_library == NULL) {
    throw FtsClientException("Can't open library");
  }

  _init_function = (void (*)(int,const char **)) getSymbol(_library, "fts_init");
  if (_init_function == NULL) {
    throw FtsClientException("Can't find initialization function");
  }

  _run_function = (void (*)(void)) getSymbol( _library, "fts_sched_run");
  if (_run_function == NULL) {
    throw FtsClientException("Can't find run function");
  }

  _halt_function = (void (*)(void)) getSymbol( _library, "fts_sched_halt");
  if (_halt_function == NULL) {
    throw FtsClientException("Can't find halt function");
  }

  _argc = args.length() + 1;
  _argv = new const char *[args.length()+1];
  
  /* add "fts" as the first argument */
  _argv[0] = "fts";

  for (i = 0; i < args.length(); i++) {
    if ( args.isString(i))
      _argv[i + 1] = args.getString( i);
  }

  _argv[_argc] = NULL;
  
#ifdef WIN32
  _thread = CreateThread(NULL, 0, &FtsPlugin::main, (LPVOID) this, 0, &threadID);
  if (_thread == NULL) {
    throw FtsClientException("Can't create thread");
  }
#else
  if (pthread_create(&_thread, NULL, &FtsPlugin::main, (void*) this)) {
    throw FtsClientException("Can't create thread");
  }
#endif
}

FtsPlugin::~FtsPlugin()
{
  if (_halt_function) {
    _halt_function();
  }
  if (_argv) {
    free(_argv);
  }
  if (_thread) {
#ifdef WIN32
    WaitForSingleObject(_thread, INFINITE);
    CloseHandle(_thread);
#else
    pthread_join( _thread, NULL);
#endif
  }
  if (_library) {
    closeLibrary( _library);
  }
}

#ifdef WIN32
DWORD WINAPI FtsPlugin::main(LPVOID data)
{
  FtsPlugin* self = (FtsPlugin*) data;
  self->_init_function(self->_argc, self->_argv);
  self->_run_function();
  ExitThread(0);
  return 0;
}
#else
void* FtsPlugin::main(void* data)
{
  FtsPlugin* self = (FtsPlugin*) data;
  self->_init_function( self->_argc, self->_argv);
  self->_run_function();

  pthread_exit(NULL);
  return NULL;
}
#endif

library_t 
FtsPlugin::openLibrary(char* name)
{
#ifdef WIN32
  return LoadLibrary(name);
#else
  return dlopen(name, RTLD_NOW | RTLD_GLOBAL);
#endif
}

void 
FtsPlugin::closeLibrary(library_t lib)
{
#ifdef WIN32
  FreeLibrary(lib);
#else
  dlclose(lib);
#endif
}

library_symbol_t 
FtsPlugin::getSymbol(library_t lib, char* name)
{	
#ifdef WIN32
  return GetProcAddress(lib, name);
#else
  return dlsym(lib, name);
#endif
}


