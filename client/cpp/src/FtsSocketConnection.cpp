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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <fts/ftsclient.h>

namespace ircam {
namespace fts {
namespace client {

#ifndef WIN32
#define INVALID_SOCKET -1
#endif

  const int FtsSocketConnection::DEFAULT_PORT = 2023;
  const int FtsSocketConnection::DEFAULT_CONNECT_TIMEOUT = 30;

#if defined(WIN32)
  void FtsSocketConnection::initializeSocketLayer()
  {
    static int initializedSocketLayer = 0;

    if (initializedSocketLayer)
      return;

    initializedSocketLayer = 1;

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
  }
#endif

  int FtsSocketConnection::connectOnce() throw( FtsClientException )
  {
    struct sockaddr_in server_addr;
    struct hostent *hostptr;
    struct in_addr addr;

    /* convert the host name to the inet address. call gethostbyname
       only when the host is not in a numbers-and-dots notation. this
       avoids a name resolution on windows machines */
    addr.s_addr = std::inet_addr( _hostname);

    if (addr.s_addr == INADDR_NONE) {

      /* host is not a numbers-and-dots notation. resolve the name. */
      hostptr = std::gethostbyname( _hostname);

      if ( !hostptr)
	throw FtsClientException( "Unknown host");

      addr = *(struct in_addr *)hostptr->h_addr_list[0];
    }

    _socket = std::socket( PF_INET, SOCK_STREAM, 0);

    if (_socket == INVALID_SOCKET)
      throw FtsClientException( "Can't create socket", errno);

    std::memset( &server_addr, 0, sizeof( server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = addr;
    server_addr.sin_port = std::htons( _port);

    if (std::connect( _socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0)
      return 0;

#ifdef WIN32
    std::closesocket( _socket);
#else
    std::close( _socket);
#endif

    _socket = INVALID_SOCKET;

    return -1;
  }

  void FtsSocketConnection::connect() throw( FtsClientException )
  {
    int r, connectTimeout = _connectTimeout;

    do
      {
	if ((r = connectOnce()) == 0)
	  break;

	sleep( 1);
	connectTimeout--;
      }
    while (connectTimeout > 0);

    if ( r < 0)
      throw FtsClientException( "Cannot connect", errno);
  }

  FtsSocketConnection::FtsSocketConnection( const char *hostname, int port, int connectTimeout) throw( FtsClientException)
    : _socket( INVALID_SOCKET), _hostname( hostname), _port( port), _connectTimeout( connectTimeout)
  {
#ifdef WIN32
    initializeSocketLayer();
#endif

    connect();
    startThread();
  }

  void FtsSocketConnection::close() throw (FtsClientException)
  {
  }

  int FtsSocketConnection::read( unsigned char *b, int len) throw (FtsClientException)
  {
    int r;

    r = std::read( _socket, b, len);
    
    if (r == -1)
      throw FtsClientException( "Error in message receiving", errno);
    else if (r == 0)
      throw FtsClientException( "Socket closed");

    return r;
  }

  void FtsSocketConnection::write( const unsigned char *b, int len) throw (FtsClientException)
  {
    if ( std::write( _socket, b, len) < 0)
      {
	throw FtsClientException( "Error in sending message", errno);
      }
  }

};
};
};
