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

namespace ircam {
namespace fts {
namespace client {

  template <class KeyT, class ValT> class FTSCLIENT_API Hashtable;
  class BinaryProtocolDecoder;
  class BinaryProtocolEncoder;

  class FTSCLIENT_API FtsServerConnection {
  public:
    static const int DEFAULT_RECEIVE_BUFFER_SIZE;
    static const int CLIENT_OBJECT_ID;

    FtsServerConnection() throw( FtsClientException);
    ~FtsServerConnection();

    // FIXME
    int getNewObjectID() { int id = _newObjectID; _newObjectID += 2; return id; }

    FtsObject *getObject( int id);
    void putObject( int id, FtsObject *obj);

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

    virtual void close() throw (FtsClientException) = 0;
    
    void startThread() throw(FtsClientException);

  protected:
    virtual int read( unsigned char *b, int len) throw (FtsClientException) = 0;
    virtual void write( const unsigned char *b, int len) throw (FtsClientException) = 0;

  private:
    static void *receiveThread( void *arg);

    int _newObjectID;
    Hashtable< int, FtsObject *> *_objectTable;
    BinaryProtocolEncoder *_encoder;
    BinaryProtocolDecoder *_decoder;
    unsigned char *_receiveBuffer;
    pthread_t _receiveThread;
  };

};
};
};
