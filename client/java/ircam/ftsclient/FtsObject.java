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

class CallbackEntry {
  CallbackEntry( Class cl, String selector)
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
    if ( ! (obj instanceof CallbackEntry))
      return false;

    return ((CallbackEntry)obj).cl.equals(cl) && ((CallbackEntry)obj).selector.equals( selector);
  }

  Class cl;
  String selector;
}

public class FtsObject {

  public FtsObject( FtsServer server, FtsObject parent, String ftsClassName) throws IOException
  {
    this.server = server;

    id = server.getNewObjectID();
    server.putObject( id, this);

    server.write( server.remote);
    server.write( "new_object");
    server.write( parent);
    server.write( id);
    server.write( ftsClassName);
    server.flush();
  }

  public FtsObject( FtsServer server, FtsObject parent, String ftsClassName, FtsArgs args) throws IOException
  {
    this.server = server;

    id = server.getNewObjectID();
    server.putObject( id, this);

    server.write( server.remote);
    server.write( "new_object");
    server.write( parent);
    server.write( id);
    server.write( ftsClassName);
    server.write( args);
    server.flush();
  }

  FtsObject( FtsServer server, int id)
  {
    this.server = server;
    this.id = id;

    server.putObject( id, this);
  }

  public void send( String selector, FtsArgs args) throws IOException
  {
    server.write( this);
    server.write( selector);
    server.write( args);
    server.flush();
  }

  public void send( String selector) throws IOException
  {
    server.write( this);
    server.write( selector);
    server.flush();
  }

  public void send( FtsArgs args) throws IOException
  {
    server.write( this);
    server.write( "list");
    server.write( args);
    server.flush();
  }

  public void send( int n) throws IOException
  {
    server.write( this);
    server.write( "int");
    server.write( n);
    server.flush();
  }

  public void send( float f) throws IOException
  {
    server.write( this);
    server.write( "float");
    server.write( f);
    server.flush();
  }

  public static void registerCallback( Class cl, String selector, FtsCallback callback)
  {
    if (selector == null)
      throw new NullPointerException();

    callbacksTable.put( new CallbackEntry( cl, selector), callback);
  }

  public static void registerCallback( Class cl, FtsCallback callback)
  {
    callbacksTable.put( new CallbackEntry( cl, "*"), callback);
  }

  static void invokeCallback( FtsObject obj, String selector, FtsArgs args)
  {
    if (selector == obj.selectorCache)
      {
	// Since we never cache the "catchAll" callback, we are sure here that 
	// the cached callback is associated with a valid selector, hence we
	// must call the first method of the callback interface 
	obj.callbackCache.invoke( obj, args.getLength(), args.getAtoms());
	return;
      }

    lookupEntry.cl = obj.getClass();
    lookupEntry.selector = selector;

    FtsCallback callback = (FtsCallback)callbacksTable.get( lookupEntry);

    if (callback != null)
      {
	obj.selectorCache = selector;
	obj.callbackCache = callback;
	obj.callbackCache.invoke( obj, args.getLength(), args.getAtoms());
	return;
      }

    lookupEntry.selector = "*";
    callback = (FtsCallback)callbacksTable.get( lookupEntry);

    if (callback != null)
      callback.invoke( obj, selector, args.getLength(), args.getAtoms());
  }

  int getID()
  {
    return id;
  }

  private int id;
  private FtsServer server;

  String selectorCache;
  FtsCallback callbackCache;

  private static Hashtable callbacksTable = new Hashtable();
  private static CallbackEntry lookupEntry = new CallbackEntry( null, null);
}
