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

package ircam.fts.client;

import java.io.*;
import java.util.*;

class MessageHandlerEntry {
  MessageHandlerEntry( Class cl, FtsSymbol selector)
  {
    this.cl = cl;
    this.selector = selector;
  }

  public int hashCode()
  {
    return cl.hashCode() + selector.hashCode();
  }

  public boolean equals( Object obj)
  {
    if ( ! (obj instanceof MessageHandlerEntry))
      return false;

    return ((MessageHandlerEntry)obj).cl.equals(cl) && ((MessageHandlerEntry)obj).selector == selector;
  }

  Class cl;
  FtsSymbol selector;
}

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
}

