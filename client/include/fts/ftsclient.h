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

#ifndef _FTSCLIENT_H_
#define _FTSCLIENT_H_

#ifdef WIN32
#include <windows.h>

#if defined(FTSCLIENT_EXPORTS)
#define FTSCLIENT_API __declspec(dllexport)
#else
#define FTSCLIENT_API __declspec(dllimport)
#endif

#else

#define FTSCLIENT_API

#endif

#include <iostream.h>

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

#include <fts/client/version.h>
#include <fts/client/types.h>
#include <fts/client/utils.h>
#include <fts/client/atom.h>
#include <fts/client/fts.h>
#include <fts/client/connection.h>
#include <fts/client/server.h>
#include <fts/client/object.h>
#include <fts/protocol.h>


#endif
