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

#ifdef WIN32

#include <windows.h>

typedef HANDLE thread_t;
typedef HINSTANCE library_t;
typedef FARPROC library_symbol_t;
typedef SOCKET socket_t;
typedef HANDLE pipe_t;

#define INVALID_PIPE NULL

/* some macros to simple life a bit */
#define CLOSESOCKET(_s)  ::closesocket(_s)
#define SOCKETWRITE(_s,_b,_n)  ::send(_s,(const char*)_b,_n,0)
#define SOCKETREAD(_s,_b,_n)  ::recv(_s,(char*)_b,_n,0)

#else

#include <pthread.h>

typedef pthread_t thread_t;
typedef void* library_t;
typedef void* library_symbol_t;
typedef int socket_t;
typedef int pipe_t;

#define INVALID_PIPE -1
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

/* Linux macros  */
#define CLOSESOCKET(_s)  ::close(_s)
#define SOCKETWRITE(_s,_b,_n)  ::write(_s,_b,_n)
#define SOCKETREAD(_s,_b,_n)  ::read(_s,_b,_n)


#endif
