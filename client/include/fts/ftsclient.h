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

#ifndef _JMAX_CLIENT_H_
#define _JMAX_CLIENT_H_

#ifdef WIN32
#include <windows.h>

#if defined(FTSCLIENT_EXPORTS)
#define FTSCLIENT_API __declspec(dllexport)
#else
#define FTSCLIENT_API __declspec(dllimport)
#endif

typedef SOCKET           socket_t;
typedef HANDLE           thread_t;
typedef HINSTANCE        library_t;
typedef FARPROC          library_symbol_t;

#else
#include <pthread.h>
#define FTSCLIENT_API

typedef int              socket_t;
typedef pthread_t        thread_t;
typedef void*            library_t;
typedef void*            library_symbol_t;

#endif


#include <iostream.h>
#include <string.h>
#include <fts/client-version.h>
#include <fts/protocol.h>

// Forward declarations
class FtsObject;
class FtsValue;
class FtsArgs;
class Fts;

FTSCLIENT_API void ftsclient_log(char* format, ...);

/**
 * Fts exception
 */
class FTSCLIENT_API FtsClientException {
public:
  FtsClientException( const char *message, int err = 0) : _message(message), _err( err) {}
  ostream &print( ostream &os) const;
  const char* getMessage() { return _message; }

private:
  const char *_message;
  int _err;
};

ostream &operator<<( ostream &os, const FtsClientException &e);

/**
 * A template buffer
 */

#define FTSBUFFER_INITIAL_SIZE  8

template <class T> class FTSCLIENT_API FtsBuffer {
public:
  FtsBuffer() { 
    _size = FTSBUFFER_INITIAL_SIZE; 
    _buffer = new T[_size]; 
    _current = 0; 
  }

  ~FtsBuffer() { 
    delete [] _buffer; 
  }

  FtsBuffer &operator++() { 
    ensureCapacity(); 
    _current++; 
    return *this; 
  }

  FtsBuffer &operator++( int unused) { 
    ensureCapacity(); 
    _current++; 
    return *this; 
  }

  T& operator[]( int index) { 
    return _buffer[index]; 
  }

  T &operator*() { 
    return _buffer[_current]; 
  }

  T* operator->() { 
    return _buffer + _current; 
  }

  operator const T*() { 
    return _buffer; 
  }

  void clear() { 
    _current = 0; 
  }

  int length() { 
    return _current; 
  }

private:
  void ensureCapacity( int wanted = 1);

  int _size;
  int _current;
  T *_buffer;
};

template <class T> void FtsBuffer<T>::ensureCapacity( int wanted)
{
  if ( _current + wanted < _size)
    return;

  while (_size <= _current + wanted)
    _size *= 2;

  T *newBuffer = new T [_size];

  for ( int i = 0; i <= _current; i++)
    newBuffer[i] = _buffer[i];

  delete [] _buffer;
  _buffer = newBuffer;
}

/**
 * A template hash table
 */

extern unsigned int getNextPrime( unsigned int n);

#define FTSHASHTABLE_DEFAULT_INITIAL_CAPACITY 101
#define FTSHASHTABLE_STANDARD_LOAD_FACTOR 0.75

template <class KeyT, class ValT> 
class FTSCLIENT_API FtsHashTable;

template <class KeyT, class ValT> 
class FTSCLIENT_API FtsHashTableCell {
friend class FtsHashTable< KeyT, ValT>;
private:
  FtsHashTableCell( KeyT k, ValT v) : _key(k), _value(v), _next(0) {}
  KeyT _key;
  ValT _value;
  FtsHashTableCell *_next;
};

template <class KeyT, class ValT> 
class FTSCLIENT_API FtsHashTable {
public:
  FtsHashTable( int initialCapacity = FTSHASHTABLE_DEFAULT_INITIAL_CAPACITY);

  ~FtsHashTable() { 
    clear(); 
    delete [] _table; 
  }

  void clear();

  int put( KeyT key, ValT value);
  int get( KeyT key, ValT &value);
  int remove( KeyT key);

  void stats( ostream &os);

protected:

  unsigned int hash( const char *s);

  unsigned int hash( int k) { 
    return (unsigned int)k; 
  }

  unsigned int hash( void *p) { 
    return (unsigned int)p>>3; 
  }

  int equals( int k1, int k2) { 
    return k1 == k2; 
  }

  int equals( const char *s1, const char *s2) { 
    return strcmp(s1, s2) == 0; 
  }

  int equals( void *p1, void *p2) { 
    return p1 == p2; 
  }

  void rehash();

private:

  FtsHashTableCell< KeyT, ValT> **lookupCell( const KeyT &key);
  int getInitialCapacity( int initialCapacity);

  int _length;
  int _count;
  int _rehashCount;
  FtsHashTableCell< KeyT, ValT> **_table;
};

template <class KeyT, class ValT> 
FtsHashTable< KeyT, ValT>::FtsHashTable( int initialCapacity)
{
  _length = getNextPrime( initialCapacity);
  _count = 0;
  _rehashCount = (int)(_length * FTSHASHTABLE_STANDARD_LOAD_FACTOR);
  _table = new FtsHashTableCell< KeyT, ValT> * [_length];

  for ( int i = 0; i < _length; i++)
    _table[i] = 0;
}

template <class KeyT, class ValT> 
void FtsHashTable< KeyT, ValT>::clear()
{
  for ( int i = 0; i < _length; i++)
    {
      FtsHashTableCell< KeyT, ValT> *p, *next;

      for ( p = _table[i]; p; p = next)
	{
	  next = p->_next;
	  delete p;
	}

      _table[i] = 0;
    }

  _count = 0;
}

template <class KeyT, class ValT> 
unsigned int FtsHashTable< KeyT, ValT>::hash( const char *s)
{
  unsigned int h = *s, i = 0;

  for ( s += 1; *s != '\0'; s++)
    h = (h<<1) + *s;

  return h;
}

template <class KeyT, class ValT> 
FtsHashTableCell<KeyT, ValT> **FtsHashTable< KeyT, ValT>::lookupCell( const KeyT &key)
{
  FtsHashTableCell< KeyT, ValT> **c;

  c = &_table[ hash( key) % _length];

  while (*c && !equals( (*c)->_key, key))
    c = &(*c)->_next;

  return c;
}

template <class KeyT, class ValT> 
int FtsHashTable< KeyT, ValT>::get( KeyT key, ValT &value)
{
  FtsHashTableCell< KeyT, ValT> **c = lookupCell( key);

  if (*c)
    {
      value = (*c)->_value;
      return 1;
    }

  return 0;
}


template <class KeyT, class ValT> 
void FtsHashTable< KeyT, ValT>::rehash()
{
  unsigned int oldLength, i;
  FtsHashTableCell< KeyT, ValT> **oldTable;

  oldLength = _length;
  _length = getNextPrime( _length);
  _rehashCount = (int)(_length * FTSHASHTABLE_STANDARD_LOAD_FACTOR);

  oldTable = _table;
  _table = new FtsHashTableCell< KeyT, ValT> * [_length];

  for ( i = 0; i < _length; i++)
    _table[i] = 0;

  for ( i = 0; i < oldLength; i++)
    {
      FtsHashTableCell< KeyT, ValT> *c, *next;

      for ( c = oldTable[i]; c; c = next)
	{
	  int index = hash( c->_key) % _length;

	  next = c->_next;
	  c->_next = _table[index];
	  _table[index] = c;
	}
    }

  delete [] oldTable;
}


template <class KeyT, class ValT>
int FtsHashTable<KeyT,ValT>::put( KeyT key, ValT value)
{
  FtsHashTableCell< KeyT, ValT> **c = lookupCell( key);

  if (*c)
    {
      (*c)->_value = value;
      return 1;
    }

  *c = new FtsHashTableCell< KeyT, ValT>( key, value);

  _count++;
  if ( _count >= _rehashCount)
    rehash();

  return 0;
}

template <class KeyT, class ValT>
int FtsHashTable< KeyT, ValT>::remove( KeyT key)
{
  FtsHashTableCell< KeyT, ValT> **c = lookupCell( key);

  if (*c)
    {
      FtsHashTableCell< KeyT, ValT> *toDelete = *c;

      *c = (*c)->_next;
      delete toDelete;
      _count--;
      
      return 1;
    }

  return 0;
}

template <class KeyT, class ValT>
void FtsHashTable< KeyT, ValT>::stats( ostream &os)
{
  int minKeys = 38928392, maxKeys = 0, i;

  os << "Hashtable: length = " << _length << " count = " << _count << " rehash = " << _rehashCount << endl;

  for ( i = 0; i < _length; i++)
    {
      unsigned int nKeys = 0;
      FtsHashTableCell< KeyT, ValT> *c;

      for ( c = _table[i]; c; c = c->_next)
	nKeys++;

      if (nKeys < minKeys)
	minKeys = nKeys;
      if (nKeys > maxKeys)
	maxKeys = nKeys;

      os << "[" << i << "] " << nKeys;

      for ( c = _table[i]; c; c = c->_next)
	{
	  os << " " << c->_key;
	}

      os << endl;
    }

  os << "minimum " << minKeys << " maximum " << maxKeys << endl;
}


/**
 * The union contained in a FtsValue
 */
union FTSCLIENT_API FtsValueUnion {
  friend class FtsValue;
  friend class FtsArgs;

private:
  int _i;
  float _f;
  char *_s;
  FtsObject *_o;
};

/**
 * The value contained in a message.
 */
class FTSCLIENT_API FtsValue {
  friend class FtsArgs;

public:
  FtsValue() : _type( FtsValue::EMPTY) {}

private:
  static const int EMPTY;
  static const int INT;
  static const int FLOAT;
  static const int STRING;
  static const int OBJECT;

  void set( int i) { 
    _type = FtsValue::INT; 
    _value._i = i; 
  }

  void set( float f) { 
    _type = FtsValue::FLOAT; 
    _value._f = f; 
  }

  void set( const char *s) { 
    _type = FtsValue::STRING; 
    _value._s = strcpy( new char[strlen(s)+1], s); 
  }

  void set( FtsObject *o) { 
    _type = FtsValue::OBJECT; 
    _value._o = o; 
  }

  void unset() { 
    if (_type == FtsValue::STRING) 
      delete [] _value._s; 
  }

  int _type;
  FtsValueUnion _value;
};

/**
 * The arguments of a message.<BR>
 * Used for object creation, message sending and as argument of callbacks.
 */
class FTSCLIENT_API FtsArgs {

public:
  // Empty constructor
  FtsArgs() {}

  // Copy constructor
  FtsArgs( const FtsArgs &args);

  ~FtsArgs() { 
    clear(); 
  }

  /**
   * add an argument of specified type at end of arguments list
   */
  void add( int i) { _buffer->set( i); _buffer++; }
  void add( float f) { _buffer->set( f); _buffer++; }
  void add( const char *s) { _buffer->set( s); _buffer++; }
  void add( FtsObject *o) { _buffer->set( o); _buffer++; }

  /**
   * tests argument type
   */
  int isInt( int index) { return _buffer[index]._type == FtsValue::INT; }
  int isFloat( int index) { return _buffer[index]._type == FtsValue::FLOAT; }
  int isString( int index) { return _buffer[index]._type == FtsValue::STRING; }
  int isObject( int index) { return _buffer[index]._type == FtsValue::OBJECT; }

  /**
   * gets an argument by index
   */
  int getInt( int index) { return _buffer[index]._value._i; }
  float getFloat( int index) { return _buffer[index]._value._f; }
  const char *getString( int index) { return _buffer[index]._value._s; }
  FtsObject *getObject( int index) { return _buffer[index]._value._o; }

  /** 
   * resets the arguments counter
   */
  void clear();

  /**
   * returns the length of arguments list
   */
  int length() { return _buffer.length(); }

  /**
   * prints content
   */
  ostream &print( ostream &os);

private:
  FtsBuffer<FtsValue> _buffer;
};

ostream &operator<<( ostream &os, FtsArgs &args);


class FTSCLIENT_API FtsServer {
  friend class FtsObject;

public:
  FtsServer();
  ~FtsServer();

  // Accessor methods for initialization parameters
  void setHostname( const char *hostname) { _hostname = strcpy( new char[strlen( hostname)+1], hostname); }
  const char *getHostname() { return _hostname; }

  /**
   * "port" property
   *
   * This property determines the port number used to connect to FTS
   * Default value: 2023
   */
#define FTSSERVER_DEFAULT_PORT 2023
  void setPort( int port) { _port = port; }
  int getPort() { return _port; }

  /**
   * "connectTimeout" property
   *
   * This property determines the timeout in seconds when trying to connect to FTS
   * Default value: 30
   */
#define FTSSERVER_DEFAULT_CONNECT_TIMEOUT 30 /* seconds */
  void setConnectTimeout( int connectTimeoutInSeconds) { _connectTimeout = connectTimeoutInSeconds; }
  int getConnectTimeout() { return _connectTimeout; }

  /**
   * "threaded" property
   *
   * This property determines if a thread is started for receiving
   * messages from FTS.
   * Default value: 1
   */
  void setThreaded( int threaded) { _threaded = threaded; }
  int isThreaded() { return _threaded; }

  /**
   * "connectTimeout" property
   *
   * This property determines the timeout in seconds when trying to connect to FTS
   * We use a big receive buffer in order to guarantee that all the messages
   * coming from FTS will be read in one read to the socket
   * Default value: 65536
   */
#define FTSSERVER_DEFAULT_RECEIVE_BUFFER_SIZE 65536
  void setReceiveBufferSize( int receiveBufferSize);
  int getReceiveBufferSize() { return _receiveBufferSize; }

  /**
   * Establish the real connection
   *
   * Tries to connect (using connect() socket function) to FTS.
   * Retries until connection is made or timeout (the timeout
   * being set by setConnectTimeout().
   * Then starts the receive thread if needed.
   */
  void connect() throw( FtsClientException);

  /**
   * Close the connection.
   *
   * Closes the connection socket and wait for end of receive thread execution.
   * Closing the socket will make the receive thread exit.
   * FIXME: this will trig an exception in the receive thread, which makes it
   * impossible to distinguish true exceptions (FTS has quitted) from termination.
   */
  void disconnect() throw( FtsClientException);

  /**
   * Shutdown the server
   *
   * Send a "shutdown" message to remote FTS.
   * This message will halt the FTS scheduler and make FTS exit.
   */
  void shutdown() throw( FtsClientException);

  /**
   * Receive messages from FTS.
   *
   * This method does a blocking read on the socket to read bytes
   * and then calls the finite state machine for protocol decoding.
   * This will in turn call the installed callbacks on the objects.
   */
  void receive() throw( FtsClientException);

  /**
   * Receive messages from FTS, non-blocking version.
   *
   * This method does a non-blocking select() on the socket and if the
   * socket is ready, calls receive().
   */
  void poll() throw( FtsClientException);

  /**
   * Wait for completion of receive thread
   * Blocks until the thread has exited.
   * (FIXME: add a timeout)
   */
  void wait() throw( FtsClientException);

  FtsObject *getObject( int id);

  // Objects with predefined IDs
  FtsObject *getRemote() { return _remote; }
  FtsObject *getRoot() { return _root; }

private:
  static const int DEFAULT_PORT;
  static const int DEFAULT_CONNECT_TIMEOUT;
  static const int DEFAULT_RECEIVE_BUFFER_SIZE;

  // flag and method for static initialization 
  static int _initialized;
  static void initialize();

  // Internal method used by connect()
  int connectOnce() throw( FtsClientException);

  // These methods are accessed only from friend class FtsObject
  void startMessage();
  void encode( int n);
  void encode( float f);
  void encode( const char *s);
  void encode( const FtsObject *o);
  void encode( FtsArgs &args);
  void endMessage() throw( FtsClientException);

  // The finite state machine for protocol decoding
  void decode( unsigned char *buffer, int size) throw( FtsClientException);

  // This method is shared by several encode() methods
  void put( int n);

  // Methods used by protocol decoding finite state machine
  void aEndObject();
  void aEndSelector();
  void aEndIntArg();
  void aEndFloatArg();
  void aEndStringArg();
  void aEndObjectArg();
  void aEndMessage();

  // The receive thread function (as a static method)
#if defined(WIN32)
  static DWORD WINAPI receiveThread(LPVOID arg);
#else
  static void *receiveThread( void *arg);
#endif

  socket_t _socket;
  thread_t _receiveThread;

  // Initialization parameters
  const char *_hostname;
  int _port;
  int _connectTimeout;
  int _threaded;
  int _receiveBufferSize;

  // Output to FTS and protocol encoding
  FtsBuffer<char> _message;

  // Input from FTS and protocol decoding
  unsigned char *_receiveBuffer;
  unsigned char _incoming;
  int _state;
  FtsObject *_object;
  FtsArgs* _args;
  FtsBuffer<char> _selector;
  FtsBuffer<char> _buff;
  int _ival;

  // System objects with predefined IDs
  FtsObject *_remote;
  FtsObject *_root;

  int _newObjectId;

  // Object table
  FtsHashTable< int, FtsObject *> _objectTable;

  // Server state;
  enum { NOT_CONNECTED = 1, CONNECTED, DISCONNECTED} _serverState;
};


/**
 * A FtsObject is the client representation of an object residing in
 * the server.
 * It allows creation and message sending.
 * A client can only send messages to the objects that it has created.
 */

class FTSCLIENT_API FtsCallback {
public:
/*    virtual void invoke( FtsArgs &args) = 0; */
/*    virtual void invoke( const char *selector, FtsArgs &args) = 0; */
  virtual void invoke( FtsArgs* args) = 0;
  virtual void invoke( const char *selector, FtsArgs* args) = 0;
};

class FTSCLIENT_API FtsObject {
  friend class FtsServer;
public:
  FtsObject( FtsServer *server, FtsObject *parent, const char *ftsClassName) throw( FtsClientException);
  FtsObject( FtsServer *server, FtsObject *parent, const char *ftsClassName, FtsArgs &args) throw( FtsClientException);
  ~FtsObject() throw( FtsClientException);

  void destroy() throw( FtsClientException);

  /**
   * returns object's id
   */
  int getID() const { return _id; }

  /**
   * connect to another object
   */
  void connectTo( int outlet, FtsObject *dst, int dstInlet);

  /**
   * connect from another object
   */
  void connectFrom( int inlet, FtsObject *src, int srcOutlet);

  /**
   * send a message<BR>
   * Most general forms of message sending.<BR>
   * Atomic forms are preferred.
   */
  void send( const char *selector, FtsArgs &args) throw( FtsClientException);
  void send( const char *selector) throw( FtsClientException);

  /**
   * send a "list" message
   */
  void send( FtsArgs &args) throw( FtsClientException);

  /**
   * send atomic messages
   */
  void send( int n) throw( FtsClientException);
  void send( float f) throw( FtsClientException);

  /**
   * installs a callback on this object
   */
  void install( const char *s, FtsCallback *callback);
  void install( FtsCallback *callback);

private:
  // This constructor is used to create the pre-defined objects
  FtsObject( FtsServer *server, int id);

  static int _newObjectId;

  FtsServer *_server;
  int _id;
  FtsHashTable< const char *, FtsCallback *> *_callbacks;
};


/**
 * An instance of class Fts represents a running FTS that
 * can execute either in a thread, in a process on in a 
 * remote process started by rsh or ssh.
 *
 * This class does not directly link to the FTS dynamic library,
 * but uses the dynamic linker, so that you can link a client
 * with the FTS client library (this one, libftsclient.so or 
 * FTSCLIENT.DLL) without having installed the FTS library (FTS.DLL
 * or libfts.so)
 */
class FTSCLIENT_API Fts {
 public:
  virtual ~Fts() {}
  virtual void run( FtsArgs &args) throw( FtsClientException) = 0;
  void run() throw( FtsClientException) { FtsArgs args; run( args); }
};

class FTSCLIENT_API FtsProcess : public Fts {
 public:
  FtsProcess( const char *path = 0);
  virtual ~FtsProcess() {}
  virtual void run( FtsArgs &args) throw( FtsClientException);

 private:
  void findDefaultPath() throw( FtsClientException);

  const char *_path;

#ifndef WIN32
  int _childPid;
#endif
};

class FTSCLIENT_API FtsPlugin : public Fts {
 public:
  FtsPlugin();
  virtual ~FtsPlugin();
  virtual void run( int argc, const char **argv) throw( FtsClientException);

 private:

#ifdef WIN32
  static DWORD WINAPI main(LPVOID data);
#else
  static void* main(void* data);
#endif

  /* utility function to load the fts library */
  static library_t openLibrary(char* name);
  static void closeLibrary(library_t lib);
  static library_symbol_t getSymbol(library_t lib, char* name);

  void (*init_function)(int argc, char **argv);
  void (*run_function)(void);
  void (*halt_function)(void);

  library_t library;
  thread_t thread;
  int argc;
  char **argv;
};

#endif
