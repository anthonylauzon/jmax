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

  class MessageHandlerEntry {
  public:
    FtsClass *_ftsClass;
    FtsSymbol *_selector;
  };

  int hash( MessageHandlerEntry &m)
  {
    return reinterpret_cast<int>(m._ftsClass)  + reinterpret_cast<int>(m._selector);
  }

  FtsObject::FtsObject(FtsServer* server, FtsObject* parent, FtsSymbol* ftsClassName) throw(FtsClientException)
  {
    _server = server;
    _parent = parent;
    _encoder = server->getEncoder();
    _id = server->getNewObjectID();
    server->putObject(_id, this);

    _encoder->writeObject(FtsServer::CLIENT_OBJECT_ID);
    _encoder->writeSymbol( FtsSymbol::sNewObject);

    _encoder->writeObject(parent);
    _encoder->writeInt(_id);
    _encoder->writeSymbol(ftsClassName);
    _encoder->flush();

  }

  FtsObject::FtsObject(FtsServer* server, FtsObject* parent, FtsSymbol* ftsClassName, FtsArgs* args) throw(FtsClientException)
  {
    _server = server;
    _parent = parent;
    _encoder = server->getEncoder();
	
    _id = server->getNewObjectID();
    server->putObject(_id, this);

    _encoder->writeObject(FtsServer::CLIENT_OBJECT_ID);
    _encoder->writeSymbol( FtsSymbol::sNewObject);
    _encoder->writeObject(parent);
    _encoder->writeInt(_id);
    _encoder->writeSymbol(ftsClassName);
    _encoder->writeArgs(*args);
    _encoder->flush();
  }

  FtsObject::FtsObject(FtsServer* server, FtsObject* parent, int id)
  {
    _server = server;
    _parent = parent;
	
    _encoder = server->getEncoder();
	
    _id = id;
    if (_id != NO_ID)
      {
	server->putObject(_id, this);
      }
  }

  void FtsObject::send(FtsSymbol* selector, FtsArgs* args) throw(FtsClientException)
  {
    _encoder->writeObject(this);
    _encoder->writeSymbol(selector);
    _encoder->writeArgs(*args);
    _encoder->flush();
  }

  void FtsObject::send(FtsSymbol* selector) throw(FtsClientException)
  {
    _encoder->writeObject(this);
    _encoder->writeSymbol(selector);
    _encoder->flush();
  }

  void FtsObject::send(FtsArgs* args) throw(FtsClientException)
  {
    _encoder->writeObject(this);
    _encoder->writeSymbol( FtsSymbol::sList);
    _encoder->writeArgs(*args);
    _encoder->flush();
  }

  void FtsObject::send(int n) throw(FtsClientException)
  {
    _encoder->writeObject(this);
    _encoder->writeSymbol( FtsSymbol::sInt);
    _encoder->writeInt(n);
    _encoder->flush();
  }

  void FtsObject::send(double d) throw(FtsClientException)
  {
    _encoder->writeObject(this);
    _encoder->writeSymbol( FtsSymbol::sFloat);
    _encoder->writeDouble(d);
    _encoder->flush();
  }

  void FtsObject::sendProperty(FtsArgs* args) throw(FtsClientException)
  {
    _encoder->writeObject(FtsServer::CLIENT_OBJECT_ID);
    _encoder->writeSymbol(FtsSymbol::get("set_object_property"));
    _encoder->writeObject(this);
    _encoder->writeArgs(*args);
    _encoder->flush();

  }

  void FtsObject::registerMessageHandler(FtsClass* ftsClass, FtsSymbol* selector, FtsMessageHandler* messageHandler)
  {
    if (0 == selector)
      {
	// throw an exception here
      }

    //    messageHandlersTable->put(new MessageHandlerEntry(ftsClass, selector), messageHandler);
  }

};
};
};

