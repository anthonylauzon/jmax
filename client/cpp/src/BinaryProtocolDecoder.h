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

#include <fts/ftsclient.h>

#include "Buffer.h"
#include "SymbolCache.h"

namespace ircam {
namespace fts {
namespace client {

  class FtsServer;

  class BinaryProtocolDecoder {

  public:

    BinaryProtocolDecoder( FtsServer *server);

    void decode( const char *data, int offset, int length) throw( FtsClientException);

  private:

    void clearAction( int input);
    void shiftAction( int input);
    void shiftLongAction( int input);
    void bufferClearAction( int input);
    void clearAllAction( int input);
    void bufferShiftAction( int input);
    void endIntAction( int input);
    void endFloatAction( int input);
    void endSymbolIndexAction( int input);
    void endSymbolCacheAction( int input);
    void endStringAction( int input);
    void endObjectAction( int input);
    void endMessageAction( int input);

    void nextState( int input);

  private:

    FtsServer *_server;
    FtsServerConnection *_connection;

    long _lval;
    Buffer *_buffer;

    FtsObject *_target;
    const FtsSymbol *_selector;
    FtsArgs *_args;
    int _argsCount;

    int _currentState;

    SymbolCache *_symbolCache;
  };

};
};
};
