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
import java.net.*;
import java.util.*;

class ReceiveThread extends Thread {
  ReceiveThread( FtsServer server)
  {
    this.server = server;
  }

  public void run()
  {
    try
      {
	while ( true)
	  server.receive();
      }
    catch (Exception e)
      {
	return;
      }
  }

  private FtsServer server;
}

/**
 *
 */

public class FtsServer {


  // We don't use java.io.BufferedOutputStream here because it does not resize
  // the buffer, and in case of an UDP connection, we want to be sure that the
  // formatted message will be send in one packet.

  class OutputBuffer {
    OutputBuffer()
    {
      length = 256;
      buffer = new byte[length];
      current = 0;
    }

    void clear()
    {
      current = 0;
    }

    final void append( byte b)
    {
      if (current + 1 >= length)
	{
	  length *= 2;
	  byte[] newBuffer = new byte[length];
	  System.arraycopy( buffer, 0, newBuffer, 0, current);
	  buffer = newBuffer;
	}

      buffer[current++] = b;
    }

    byte[] getBytes()
    {
      return buffer;
    }

    int getLength()
    {
      return current;
    }

    private byte[] buffer;
    private int current;
    private int length;
  }

  public FtsServer( FtsServerConnection connection)
  {
    this.connection = connection;

    newObjectID = 16; // Ids 0 to 15 are reserved for pre-defined system objects
    objectTable = new Hashtable();
    outputBuffer = new OutputBuffer();

    inputBuffer = new byte[0x10000];

    decoder = new FtsProtocolDecoder( this);

    root = new FtsObject( this, 0);
    remote = new FtsObject( this, 1);

    receiveThread = new ReceiveThread( this);
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
  void shutdown() throws FtsClientException
  {
  }

  /**
   * Receive messages from FTS.
   *
   * This method does a blocking read on the socket to read bytes
   * and then calls the finite state machine for protocol decoding.
   * This will in turn call the installed callbacks on the objects.
   */
  void receive() throws IOException,FtsClientException
  {
    int len = connection.read( inputBuffer, 0, inputBuffer.length);

    decoder.decode( inputBuffer, 0, len);
  }

  private final void put( int i)
  {
    outputBuffer.append( (byte) ((i >> 24) & 0xff));
    outputBuffer.append( (byte) ((i >> 16) & 0xff));
    outputBuffer.append( (byte) ((i >> 8) & 0xff));
    outputBuffer.append( (byte) ((i >> 0) & 0xff));
  }

  void write( int i)
  {
    outputBuffer.append( FtsProtocol.INT);
    put( i);
  }

  void write( float f)
  {
    outputBuffer.append( FtsProtocol.FLOAT);
    put( Float.floatToRawIntBits(f));
  }

  void write( String s)
  {
    outputBuffer.append( FtsProtocol.STRING);

    for ( int i = 0; i < s.length(); i++)
      outputBuffer.append( (byte)s.charAt(i));

    outputBuffer.append( FtsProtocol.STRING_END);
  }

  void write( FtsObject o)
  {
    outputBuffer.append( FtsProtocol.OBJECT);

    if (o != null)
      put( o.getID());
    else
      put( 0);
  }

  void write( FtsAtom[] atoms, int offset, int length)
  {
    for ( int i = offset; i < length; i++)
      {
	if ( atoms[i].isInt())
	  write( atoms[i].intValue);
	else if ( atoms[i].isFloat())
	  write( atoms[i].floatValue);
	else if ( atoms[i].isString())
	  write( atoms[i].stringValue);
	else if ( atoms[i].isObject())
	  write( atoms[i].objectValue);
      }
  }

  void write( FtsArgs args)
  {
    write( args.getAtoms(), 0, args.getLength());
  }

  void flush() throws IOException
  {
    outputBuffer.append( FtsProtocol.END_OF_MESSAGE);

    connection.write( outputBuffer.getBytes(), 0, outputBuffer.getLength());

    outputBuffer.clear();
  }

  public FtsObject getRoot()
  {
    return root;
  }

  FtsObject getObject( int id)
  {
    return (FtsObject)objectTable.get( new Integer( id));
  }

  void putObject( int id, FtsObject object)
  {
    objectTable.put( new Integer( id), object);
  }

  int getNewObjectID()
  {
    return newObjectID++;
  }

  // Connection to FTS
  private FtsServerConnection connection;

  // Output to FTS
  private OutputStream output;
  private OutputBuffer outputBuffer;

  // Input from FTS
  private byte[] inputBuffer;
  private FtsProtocolDecoder decoder;
  private Thread receiveThread;

  // Proxies of remote root and client
  FtsObject root;
  FtsObject remote;

  // Objects ID handling
  private int newObjectID;
  private Hashtable objectTable;
}

