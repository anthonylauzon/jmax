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

// Forward declarations
class FtsObject;

class FTSCLIENT_API FtsServer {
  friend class FtsObject;

public:
  static const int DEFAULT_RECEIVE_BUFFER_SIZE;

  FtsServer( FtsServerConnection *connection, int threaded = 1);

  ~FtsServer();

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
   * This method calls connection poll() method and if the connection 
   * is ready, calls receive().
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
  // These methods are accessed only from friend class FtsObject
  // This method is shared by several encode() methods
  void put( int n);

  void startMessage();
  void encode( int n);
  void encode( float f);
  void encode( const char *s);
  void encode( const FtsObject *o);
  void encode( FtsArgs &args);
  void endMessage() throw( FtsClientException);

  // The finite state machine for protocol decoding
  void decode( unsigned char *buffer, int size) throw( FtsClientException);

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

  client_thread_t _receiveThread;

  // Connection to FTS
  FtsServerConnection *_connection;

  // Output to FTS and protocol encoding
  FtsBuffer<unsigned char> _message;

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

