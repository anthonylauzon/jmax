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

#include <string.h>
#include <fts/client/types.h>

/**
 * An abstract class used as base class for all classes
 * representing a connection to a running FTS server.
 */

class FTSCLIENT_API FtsServerConnection {
public:
  virtual void close() throw( FtsClientException) = 0;
  virtual int poll() throw( FtsClientException) = 0;
  virtual int read( unsigned char *buffer, int n) throw( FtsClientException) = 0;
  virtual int write( const unsigned char *buffer, int n) throw( FtsClientException) = 0;
};

class FTSCLIENT_API FtsSocketConnection: public FtsServerConnection {
public:

  static const int DEFAULT_PORT;
  static const int DEFAULT_CONNECT_TIMEOUT;

  // flag and method for static initialization of the socket layer
  // (windows only)
  static int _initializedSocketLayer;
  static void initializeSocketLayer();

  /**
     Tries to connect (using connect() socket function) to FTS.
     Retries until connection is made or timeout is reached.

     @param hostname the hostname to which the connection is established
     @param port the port number used to connect to FTS
     @param connectTimeout the timeout in seconds when trying to connect to FTS
  */
  FtsSocketConnection( const char *hostname, int port, int connectTimeout) throw( FtsClientException);

  FtsSocketConnection() throw( FtsClientException);

  void close() throw( FtsClientException);
  int poll() throw( FtsClientException);
  int read( unsigned char *buffer, int n) throw( FtsClientException);
  int write( const unsigned char *buffer, int n) throw( FtsClientException);

private:
  // Internal methods used by constructor
  int connectOnce() throw( FtsClientException);
  void connect() throw( FtsClientException);

  socket_t _socket;
  const char *_hostname;
  int _port;
  int _connectTimeout;
};

class FTSCLIENT_API FtsPipeConnection: public FtsServerConnection {
  friend class FtsProcess;
public:
  FtsPipeConnection( FtsProcess *fts);
  virtual ~FtsPipeConnection();

  void close() throw( FtsClientException);
  int poll() throw( FtsClientException);
  int read( unsigned char *buffer, int n) throw( FtsClientException);
  int write( const unsigned char *buffer, int n) throw( FtsClientException);

private:
  FtsProcess *_fts;
  pipe_t _in;
  pipe_t _out;
};
