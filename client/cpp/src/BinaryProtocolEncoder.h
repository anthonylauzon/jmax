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

#include "SymbolCache.h"
#include "Buffer.h"
#include "BinaryProtocol.h"

namespace ircam {
namespace fts {
namespace client {

  class BinaryProtocolEncoder {
  public:
    BinaryProtocolEncoder();
    ~BinaryProtocolEncoder();

    void writeInt( int v) throw( FtsClientException);

    void writeDouble( double v) throw( FtsClientException);

    void writeSymbol( const char *v) throw( FtsClientException);

    void writeString( const char *v) throw( FtsClientException);

    void writeRawString( const char *v) throw( FtsClientException);

    void writeObject( const FtsObject *v) throw( FtsClientException);

    /* This version is used for object that have predefined IDs */
    void writeObject( int id) throw( FtsClientException);

    void writeArgs( const FtsArgs &v) throw( FtsClientException);

    void endOfMessage() throw( FtsClientException);

    void clear() { _outputBuffer->clear(); }

    unsigned char *getBytes() { return _outputBuffer->getBytes(); }
    
    int getLength() { return _outputBuffer->getLength(); }

  private:
    void write( int v);

    void write( long v);

    void write( const char *v);

    Buffer *_outputBuffer;
    SymbolCache *_symbolCache;
  };


};
};
};
