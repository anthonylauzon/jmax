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

	_encoder->writeObject(FtsServer.CLIENT_OBJECT_ID);
	_encoder->writeSymbol(sNewObject);

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

	_encoder->writeObject(FtsServer.CLIENT_OBJECT_ID);
	_encoder->writeSymbol(sNewObject);
	_encoder->writeObject(parent);
	_encoder->writeInt(_id);
	_encoder->writeSymbol(ftsClassName);
	_encoder->writeArgs(args);
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
	_encoder->writeArgs(args);
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
	_encoder->writeSymbol(sList);
	_encoder->writeArgs(args);
	_encoder->flush();
    }

    void FtsObject::send(int n) throw(FtsClientException)
    {
	_encoder->writeObject(this);
	_encoder->writeSymbol(sInt);
	_encoder->writeInt(n);
	_encoder->flush();
    }

    void FtsObject::send(double d) throw(FtsClientException)
    {
	_encoder->writeObject(this);
	_encoder->writeSymbol(sFloat);
	_encoder->writeDouble(d);
	_encoder->flush();
    }

    void FtsObject::sendProperty(FtsArgs* args) throw(FtsClientException)
    {
	_encoder->writeObject(FtsServer.CLIENT_OBJECT_ID);
	_encoder->writeSymbol(FtsSymbol.get("set_object_property"));
	_encoder->writeObject(this);
	_encoder->writeArgs(args);
	_encoder->flush();

    }

    void FtsObject::registerMessageHandler(FtsClass* ftsClass, FtsSymbol* selector, FtsMessageHandler* messageHandler)
    {
	if (0 == selector)
	{
	    // throw an exception here
	}

	messageHandlersTable->put(new MessageHandlerEntry(ftsClass, selector), messageHandler);
    }
};
};
};

#if 0
public class FtsObject implements Serializable
{
  public static final int NO_ID = -1;

  public FtsObject( FtsServer server, FtsObject parent, FtsSymbol ftsClassName) throws IOException
  {
    this.server = server;
    this.parent = parent;
    encoder = server.getEncoder();

    id = server.getNewObjectID();
    server.putObject( id, this);

    encoder.writeObject( FtsServer.CLIENT_OBJECT_ID);
    encoder.writeSymbol( sNewObject );
    encoder.writeObject( parent);
    encoder.writeInt( id);
    encoder.writeSymbol( ftsClassName);
    encoder.flush();
  }

  public FtsObject( FtsServer server, FtsObject parent, FtsSymbol ftsClassName, FtsArgs args) throws IOException
  {
    this.server = server;
    this.parent = parent;
    encoder = server.getEncoder();

    id = server.getNewObjectID();
    server.putObject( id, this);

    encoder.writeObject( FtsServer.CLIENT_OBJECT_ID);
    encoder.writeSymbol( sNewObject );
    encoder.writeObject( parent);
    encoder.writeInt( id);
    encoder.writeSymbol( ftsClassName);
    encoder.writeArgs( args);
    encoder.flush();
  }

  public FtsObject( FtsServer server, FtsObject parent, int id)
  {
    this.server = server;
    this.parent = parent;

    encoder = server.getEncoder();

    this.id = id;
    if ( id != NO_ID)
      server.putObject( id, this);
  }

  public void send( FtsSymbol selector, FtsArgs args) throws IOException
  {
    encoder.writeObject( this);
    encoder.writeSymbol( selector);
    encoder.writeArgs( args);
    encoder.flush();
  }

  public void send( FtsSymbol selector) throws IOException
  {
    encoder.writeObject( this);
    encoder.writeSymbol( selector);
    encoder.flush();
  }

  public void send( FtsArgs args) throws IOException
  {
    encoder.writeObject( this);
    encoder.writeSymbol( sList);
    encoder.writeArgs( args);
    encoder.flush();
  }

  public void send( int n) throws IOException
  {
    encoder.writeObject( this);
    encoder.writeSymbol( sInt);
    encoder.writeInt( n);
    encoder.flush();
  }

  public void send( double d) throws IOException
  {
    encoder.writeObject( this);
    encoder.writeSymbol( sFloat);
    encoder.writeDouble( d);
    encoder.flush();
  }

  public void sendProperty(FtsArgs args) throws IOException
  {
    encoder.writeObject( FtsServer.CLIENT_OBJECT_ID);
    encoder.writeSymbol( FtsSymbol.get("set_object_property"));
    encoder.writeObject( this);
    encoder.writeArgs( args);
    encoder.flush();
  }
  
  public static void registerMessageHandler( Class cl, FtsSymbol selector, FtsMessageHandler messageHandler)
  {
    if (selector == null)
      throw new NullPointerException();

    messageHandlersTable.put( new MessageHandlerEntry( cl, selector), messageHandler);
  }

  static void invokeMessageHandler( FtsObject obj, FtsSymbol selector, FtsArgs args)
  {
    if (selector == obj.selectorCache)
      {
	obj.messageHandlerCache.invoke( obj, args);
	return;
      }

    Class cl = obj.getClass();

    lookupEntry.selector = selector;

    do
      {
	lookupEntry.cl = cl;

	FtsMessageHandler messageHandler = (FtsMessageHandler)messageHandlersTable.get( lookupEntry);

	if (messageHandler != null)
	  {
	    obj.selectorCache = selector;
	    obj.messageHandlerCache = messageHandler;
	    obj.messageHandlerCache.invoke( obj, args);
	    return;
	  }

	cl = cl.getSuperclass();
      }
    while (cl != null);
  }

  public FtsObject getParent()
  {
    return parent;
  }

  public final FtsServer getServer()
  {
    return server;
  }

  final int getID()
  {
    return id;
  }

  final void setID( int id)
  {
    this.id = id;
  }

  private int id;
  private transient FtsServer server;
  private transient BinaryProtocolEncoder encoder;

  private FtsObject parent;

  private transient FtsSymbol selectorCache;
  private transient FtsMessageHandler messageHandlerCache;

  private transient static HashMap messageHandlersTable = new HashMap();
  private transient static MessageHandlerEntry lookupEntry = new MessageHandlerEntry( null, null);

  private transient static FtsSymbol sNewObject = FtsSymbol.get( "new_object");
  private transient static FtsSymbol sDelObject = FtsSymbol.get( "delete_object");
  private transient static FtsSymbol sInt = FtsSymbol.get( "int");
  private transient static FtsSymbol sFloat = FtsSymbol.get( "float");
  private transient static FtsSymbol sList = FtsSymbol.get( "list");
}

#endif
