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

package ircam.ftsclient;

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

public class FtsObject {

  public FtsObject( FtsServer server, FtsObject parent, FtsSymbol ftsClassName) throws IOException
  {
    this.server = server;
    encoder = server.getEncoder();

    id = server.getNewObjectID();
    server.putObject( id, this);

    encoder.writeObject( server.remote);
    encoder.writeSymbol( FtsSymbol.get( "new_object"));
    encoder.writeObject( parent);
    encoder.writeInt( id);
    encoder.writeSymbol( ftsClassName);
    encoder.flush();
  }

  public FtsObject( FtsServer server, FtsObject parent, FtsSymbol ftsClassName, FtsArgs args) throws IOException
  {
    this.server = server;
    encoder = server.getEncoder();

    id = server.getNewObjectID();
    server.putObject( id, this);

    encoder.writeObject( server.remote);
    encoder.writeSymbol( FtsSymbol.get("new_object"));
    encoder.writeObject( parent);
    encoder.writeInt( id);
    encoder.writeSymbol( ftsClassName);
    encoder.writeArgs( args);
    encoder.flush();
  }

  public FtsObject( FtsServer server, int id)
  {
    this.server = server;
    encoder = server.getEncoder();

    this.id = id;

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
    encoder.writeSymbol( FtsSymbol.get("list"));
    encoder.writeArgs( args);
    encoder.flush();
  }

  public void send( int n) throws IOException
  {
    encoder.writeObject( this);
    encoder.writeSymbol( FtsSymbol.get("int"));
    encoder.writeInt( n);
    encoder.flush();
  }

  public void send( float f) throws IOException
  {
    encoder.writeObject( this);
    encoder.writeSymbol( FtsSymbol.get("float"));
    encoder.writeFloat( f);
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
	obj.messageHandlerCache.invoke( obj, args.getLength(), args.getAtoms());
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
	    obj.messageHandlerCache.invoke( obj, args.getLength(), args.getAtoms());
	    return;
	  }

	cl = cl.getSuperclass();
      }
    while (cl != null);
  }

  int getID()
  {
    return id;
  }

  private int id;
  private FtsServer server;
  private FtsProtocolEncoder encoder;

  private FtsSymbol selectorCache;
  private FtsMessageHandler messageHandlerCache;

  private static HashMap messageHandlersTable = new HashMap();
  private static MessageHandlerEntry lookupEntry = new MessageHandlerEntry( null, null);
}
