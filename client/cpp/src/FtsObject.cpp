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
#include <fts/ftsclient.h>
#include "Hashtable.h"
#include "BinaryProtocolEncoder.h"

namespace ircam {
namespace fts {
namespace client {

  Hashtable<MessageHandlerEntry, FtsMessageHandler *> FtsObject::messageHandlersTable;

  class MessageHandlerEntry {
  public:
    MessageHandlerEntry( const std::type_info &tid, const char *selector) 
      : _tid( tid), _selector( selector)
    {
    }

    const char *className() const { return _tid.name();}

    const char *selector() const { return _selector;}

    bool operator ==( const MessageHandlerEntry &rhs ) const 
    {
      return _tid == rhs._tid && equals( _selector, rhs._selector);
    }

  private:
    const std::type_info &_tid;
    const char *_selector;
  };

  unsigned int hash( const MessageHandlerEntry &m)
  {
    return hash(m.className()) + hash(m.selector());
  }

  FtsObject::FtsObject(FtsServerConnection* serverConnection, FtsObject* parent, const char* ftsClassName) throw(FtsClientException)
  {
    _selectorCache = 0;
    _serverConnection = serverConnection;
    _parent = parent;
    _id = serverConnection->getNewObjectID();
    _serverConnection->putObject(_id, this);

    _serverConnection->writeObject(FtsServerConnection::CLIENT_OBJECT_ID);
    _serverConnection->writeSymbol( "new_object");

    _serverConnection->writeObject(parent);
    _serverConnection->writeInt(_id);
    _serverConnection->writeSymbol(ftsClassName);
    _serverConnection->endOfMessage();
  }

  FtsObject::FtsObject(FtsServerConnection* serverConnection, FtsObject* parent, const char* ftsClassName, FtsArgs* args) throw(FtsClientException)
  {
    _selectorCache = 0;
    _serverConnection = serverConnection;
    _parent = parent;
	
    _id = _serverConnection->getNewObjectID();
    _serverConnection->putObject(_id, this);

    _serverConnection->writeObject(FtsServerConnection::CLIENT_OBJECT_ID);
    _serverConnection->writeSymbol( "new_object");
    _serverConnection->writeObject(parent);
    _serverConnection->writeInt(_id);
    _serverConnection->writeSymbol(ftsClassName);
    _serverConnection->writeArgs(*args);
    _serverConnection->endOfMessage();

  }

  FtsObject::FtsObject(FtsServerConnection* serverConnection, FtsObject* parent, int id)
  {
    _selectorCache = 0;
    _serverConnection = serverConnection;
    _parent = parent;
	
    _id = id;
    if (_id != NO_ID)
      {
	serverConnection->putObject(_id, this);
      }
  }

  void FtsObject::send(const char* selector, FtsArgs* args) throw(FtsClientException)
  {
    _serverConnection->writeObject(this);
    _serverConnection->writeSymbol(selector);
    if (args != NULL)
      _serverConnection->writeArgs(*args);
    _serverConnection->endOfMessage();
  }

  void FtsObject::registerMessageHandler(const std::type_info &tid, const char* selector, FtsMessageHandler* messageHandler)
  {
    MessageHandlerEntry entry( tid, selector);

    messageHandlersTable.put( entry, messageHandler);
  }

  void FtsObject::invokeMessageHandler( FtsObject *obj, const char *selector, const FtsArgs &args)
  {
    /*
      We can compare the strings using == because the protocol decoder "interns" the symbol
      that it receives
    */
    if (selector == obj->_selectorCache)
      {
	obj->_messageHandlerCache->invoke( obj, args);
	return;
      }

    MessageHandlerEntry entry( typeid(*obj), selector);

    FtsMessageHandler *messageHandler; ;

    if (messageHandlersTable.get( entry, messageHandler))
      {
	obj->_selectorCache = selector;
	obj->_messageHandlerCache = messageHandler;
	obj->_messageHandlerCache->invoke( obj, args);
	return;
      }
  }
};
};
};

