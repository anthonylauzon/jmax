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

namespace ircam {
namespace fts {
namespace client {

  class MessageHandlerEntry;
  template <class KeyT, class ValT> class FTSCLIENT_API Hashtable;

  class FTSCLIENT_API FtsObject {
    friend class BinaryProtocolEncoder;
  public:
    static const int NO_ID = -1;

    FtsObject( FtsServerConnection *serverConnection, FtsObject *parent, const char *ftsClassName) throw( FtsClientException);

    FtsObject( FtsServerConnection *serverConnection, FtsObject *parent, const char *ftsClassName, FtsArgs *args) throw( FtsClientException);

    FtsObject( FtsServerConnection *serverConnection, FtsObject *parent, int id);

    void send( const char *selector, FtsArgs *args = NULL) throw( FtsClientException);

    FtsObject *getParent()
    {
      return _parent;
    }

    FtsServerConnection *getServerConnection()
    {
      return _serverConnection;
    }

    static void registerMessageHandler( const type_info &ftsClass, const char *selector, FtsMessageHandler *messageHandler);

  private:
    void invokeMessageHandler( FtsObject *obj, const char *selector, FtsArgs *args);

    int getID() const
    {
      return _id;
    }

    void setID( int id)
    {
      _id = id;
    }

    int _id;
    FtsServerConnection *_serverConnection;

    FtsObject *_parent;

    const char *_selectorCache;
    FtsMessageHandler *_messageHandlerCache;

    static Hashtable<MessageHandlerEntry *, FtsMessageHandler *> *messageHandlersTable;
    static MessageHandlerEntry lookupEntry;
  };

};
};
};

std::ostream &operator<<( std::ostream &os, const ircam::fts::client::FtsObject &o);
