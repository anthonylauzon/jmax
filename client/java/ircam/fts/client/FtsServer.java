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
import java.net.*;
import java.util.*;

/**
 *
 */
public class FtsServer {

  class ReceiveThread extends Thread {
    public void run()
    {
      byte[] inputBuffer = new byte[0x10000];

      try
	{
	  while ( true)
	    {
	      int len = connection.read( inputBuffer, 0, inputBuffer.length);
	      decoder.decode( inputBuffer, 0, len);
	    }
	}
      catch (Exception e)
	{
	  System.err.println( "ReceiveThread: got exception " + e);
	  e.printStackTrace();
	  return;
	}
    }
  }

  class FtsClientObject extends FtsObject {
    FtsClientObject( FtsServer server, FtsObject root, int id)
    {
      super( server, root, id);
    }
  }

  static
  {
    FtsObject.registerMessageHandler( FtsClientObject.class, FtsSymbol.get( "notify"), new FtsMessageHandler() {
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsClientObject)obj).getServer().fireFtsServerEvent( args);
	}
      });
  }

  public FtsServer( FtsServerConnection connection)
  {
    this.connection = connection;

    newObjectID = 16; // Ids 0 to 15 are reserved for pre-defined system objects
    objectTable = new HashMap();

    encoder = new FtsBinaryProtocolEncoder( this);
    decoder = new FtsBinaryProtocolDecoder( this);

    root = new FtsObject( this, null, 0);
    client = new FtsClientObject( this, root, 1);

    listeners = new ArrayList();
    event = new FtsServerEvent( this);

    receiveThread = new ReceiveThread();
    receiveThread.start();
  }

  /**
   * Close the connection.
   *
   * Closes the connection socket and wait for end of receive thread execution.
   * Closing the socket will make the receive thread exit after a short while.
   */
  void close() throws FtsClientException, IOException
  {
    connection.close();
  }

  /**
   * Shutdown the server
   *
   * Send a "shutdown" message to remote FTS.
   * This message will halt the FTS scheduler and make FTS exit.
   */
  public void shutdown() throws FtsClientException
  {
  }

  public FtsObject getRoot()
  {
    return root;
  }

  public void addFtsServerListener( FtsServerListener listener)
  {
    listeners.add( listener);
  }

  public void removeFtsServerListener( FtsServerListener listener)
  {
    listeners.remove( listener);
  }


  // Non-public methods

  void fireFtsServerEvent( FtsArgs args)
  {
    for ( Iterator i = listeners.iterator(); i.hasNext(); )
      {
	FtsServerListener listener = (FtsServerListener)i.next();
	listener.messageReceived( event, args);
      }
  }

  int getNewObjectID()
  {
    int id = newObjectID;

    newObjectID += 2;

    return id;
  }

  FtsObject getClient()
  {
    return client;
  }

  FtsObject getObject( int id)
  {
    return (FtsObject)objectTable.get( new Integer( id));
  }

  void putObject( int id, FtsObject object)
  {
    objectTable.put( new Integer( id), object);
  }

  FtsProtocolEncoder getEncoder()
  {
    return encoder;
  }

  FtsServerConnection getConnection()
  {
    return connection;
  }

  // Connection to FTS
  private FtsServerConnection connection;

  // Input from FTS
  private FtsProtocolDecoder decoder;
  private Thread receiveThread;

  // Output to FTS
  private FtsProtocolEncoder encoder;

  // Proxies of remote root and client
  private FtsObject root;
  private FtsObject client;

  // Objects ID handling
  private int newObjectID;
  private HashMap objectTable;

  // FtsServerEvent handling
  private FtsServerEvent event;
  private List listeners;
}
