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

#include <typeinfo>

namespace ircam {
namespace fts {
namespace client {

  class FtsServer;
  class BinaryProtocolEncoder;
  class MessageHandlerEntry;
  class FtsClass;
  template <class KeyT, class ValT> class FTSCLIENT_API Hashtable;

  class FTSCLIENT_API FtsObject {
  public:
    FtsObject( FtsServer *server, FtsObject *parent, FtsSymbol *ftsClassName) throw( FtsClientException);

    FtsObject( FtsServer *server, FtsObject *parent, FtsSymbol *ftsClassName, FtsArgs *args) throw( FtsClientException);

    FtsObject( FtsServer *server, FtsObject *parent, int id);

    void send( FtsSymbol *selector, FtsArgs *args) throw( FtsClientException);

    void send( FtsSymbol *selector) throw( FtsClientException);

    void send( FtsArgs *args) throw( FtsClientException);

    void send( int n) throw( FtsClientException);

    void send( double d) throw( FtsClientException);

    void sendProperty(FtsArgs *args) throw( FtsClientException);
  
    FtsObject *getParent()
    {
      return _parent;
    }

    FtsServer *getServer()
    {
      return _server;
    }

    static void registerMessageHandler( FtsClass *ftsClass, FtsSymbol *selector, FtsMessageHandler *messageHandler);

  private:
    void invokeMessageHandler( FtsObject *obj, FtsSymbol *selector, FtsArgs *args);

    int getID()
    {
      return _id;
    }

    void setID( int id)
    {
      _id = id;
    }

    int _id;
    FtsServer *_server;
    BinaryProtocolEncoder *_encoder;

    FtsObject *_parent;

    FtsSymbol *_selectorCache;
    FtsMessageHandler *_messageHandlerCache;

    static Hashtable<MessageHandlerEntry *, FtsMessageHandler *> *messageHandlersTable;
    static MessageHandlerEntry lookupEntry;

    static FtsSymbol *sNewObject;
    static FtsSymbol *sDelObject;
    static FtsSymbol *sInt;
    static FtsSymbol *sFloat;
    static FtsSymbol *sList;
  };

};
};
};

std::ostream &operator<<( std::ostream &os, const ircam::fts::client::FtsObject &o);
