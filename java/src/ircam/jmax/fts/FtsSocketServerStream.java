//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
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
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.fts;

import java.io.*;
import java.net.*;

import ircam.jmax.*;

/**
 * The socket connection.
 * Implement a specialed connection using a socket (server side)
 * for communicating with FTS; start FTS thru a rsh command,
 * so it will work only on unix machines and the like.
 */

class FtsSocketServerStream extends FtsStream
{
  Socket socket;
  ServerSocket serverSocket;
  Process proc;
  InputStream in_stream = null;
  OutputStream out_stream = null;

  FtsSocketServerStream( String host, String ftsDir, String ftsName, String ftsOptions)
  {
    int port = 0;
    String command;

    // Create the server socket on the first free port

    try
      {
	serverSocket = new ServerSocket(0);// look for a free port
	port = serverSocket.getLocalPort();
      }
    catch (java.io.IOException e)
      {
	System.out.println("Error while opening server socket " + e);
      }

    try
      {
	if ( host.equals( "local") || host.equals( InetAddress.getLocalHost().getHostName()))
	  {
	    command = "";
	  }
	else
	  {
	    command = "rsh " + host + " ";
	  }

	command += ftsDir + "/" + ftsName;
	command += " socket " + InetAddress.getLocalHost().getHostAddress() + ":" + port;
	command += " " + ftsOptions;
      }
    catch (UnknownHostException e)
      {
	System.out.println("Cannot find local host");
	return;
      }

    // Run FTS remotely

    try
      {
	proc = Runtime.getRuntime().exec(command);
      }
    catch (IOException e)
      {
	System.out.println("Cannot exec command: " + command);
	return;
      }

    // Wait the connection

    try
      {
	socket = serverSocket.accept();
      }
    catch (IOException e)
      {
	System.out.println("I/O error during accept on server socket");
	return;
      }    

    try
      {
	socket.setTcpNoDelay(true);
      }
    catch (SocketException e)
      {
	System.out.println("setTcpNoDelay exception: " + e);
      } 

    // Set the in and out stream

    try
      {
	out_stream = new BufferedOutputStream(socket.getOutputStream(), 1024);
	in_stream  = new BufferedInputStream(socket.getInputStream(), 1024);

	FtsErrorStreamer.startFtsErrorStreamer(proc.getErrorStream());
      }
    catch (IOException e)
      {
	System.out.println("Couldn't get I/O for the socket ");
	return;
      }    
  }


  /** Close the connection and shutdown the server */

  void close()
  {
    FtsErrorStreamer.stopFtsErrorStreamer();

    try
      {
	in_stream.close();
	out_stream.close();
	socket.close();
	serverSocket.close();
	proc.destroy();
      } 
    catch (IOException e)
      {
	System.out.println("I/O failed on closing the serverSocket connection");
      }
  }

  /** Check if the connection is  open */

  boolean isOpen()
  {
    return (in_stream != null) && (out_stream != null);
  }


  /** Method to send a char; since we can use datagram sockets or other
    means I/O is not necessarly done thru streams */

  protected void write(int data) throws java.io.IOException
  {
    out_stream.write(data);
  }

  /** Method to receive a char; since we can use datagram sockets or other
    means I/O is not necessarly done thru streams */

  protected int read() throws java.io.IOException,  FtsQuittedException
  {
    int c;

    c = in_stream.read();

    if (c == -1)
      throw new FtsQuittedException();

    return c;
  }

  /** Method to Ask for an explicit output flush ; since we
    can use datagram sockets or other means I/O is not necessarly done
    thru streams */

  void flush() throws java.io.IOException
  {
    out_stream.flush();
  }
}




