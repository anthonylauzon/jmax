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
    ReceiveThread( BinaryProtocolDecoder decoder)
    {
      this.decoder = decoder;
    }

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

    private BinaryProtocolDecoder decoder;
  }

  public FtsServer( FtsServerConnection connection)
  {
    this.connection = connection;

    newObjectID = 16; // Ids 0 to 15 are reserved for pre-defined system objects

    objectTable = new HashMap();
    encoder = new BinaryProtocolEncoder( this.connection);
  }

  static final int ROOT_OBJECT_ID = 0;
  static final int CLIENT_OBJECT_ID = 1;

  public void setRootObject( FtsObject rootObject)
  {
    rootObject.setID( ROOT_OBJECT_ID);
    putObject( ROOT_OBJECT_ID, rootObject);
  }

  public void setClientObject( FtsObject clientObject)
  {
    clientObject.setID( CLIENT_OBJECT_ID);
    putObject( CLIENT_OBJECT_ID, clientObject);
  }

  /**
   * Start the receive thread
   *
   * As soon as this method is called, message handlers will be called.
   * This implies that the objects that may receive messages must be 
   * already installed with their message handlers.
   */
  public void start()
  {
    receiveThread = new ReceiveThread( new BinaryProtocolDecoder( this));
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

  int getNewObjectID()
  {
    int id = newObjectID;

    newObjectID += 2;

    return id;
  }

  FtsObject getObject( int id)
  {
    return (FtsObject)objectTable.get( new Integer( id));
  }

  void putObject( int id, FtsObject object)
  {
    objectTable.put( new Integer( id), object);
  }

  BinaryProtocolEncoder getEncoder()
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
  private Thread receiveThread;

  // Output to FTS
  private BinaryProtocolEncoder encoder;

  // Objects ID handling
  private int newObjectID;
  private HashMap objectTable;
}

