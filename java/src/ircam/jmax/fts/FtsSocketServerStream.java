//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.fts;

import java.io.*;
import java.net.*;

/**
 * The socket connection.
 * Implement a specialed connection using a socket (server side)
 * for communicating with FTS; start FTS thru a rsh command,
 * so it will work only on unix machines and the like.
 */

class FtsSocketServerStream extends FtsStream
{
  String host;
  int port;
  Socket socket;
  ServerSocket serverSocket;
  String path;
  String ftsName;
  Process proc;
  InputStream in_stream = null;
  OutputStream out_stream = null;

  FtsSocketServerStream(String host, String path, String ftsName)
  {
    super(host);
    this.host = host;
    this.path = path;
    this.ftsName = ftsName;

    // Create the server socket on the first free port

    try
      {
	this.serverSocket = new ServerSocket(0);// look for a free port
	this.port = serverSocket.getLocalPort();
      }
    catch (java.io.IOException e)
      {
	System.out.println("Error while opening server socket " + e);
      }

    String command;

    try
      {
	if (host.equals(InetAddress.getLocalHost().getHostName()))
	  {
	    command = (path + "/" + ftsName + ( Fts.getNoRealTime() ? " -norealtime" : "")
		       + " socket " + InetAddress.getLocalHost().getHostAddress() + ":" + port) ;
	  }
	else
	  {
	    command = ("rsh " + host + " " + path + "/" + ftsName + ( Fts.getNoRealTime() ? " -norealtime" : "")
		       + " socket " + InetAddress.getLocalHost().getHostAddress() + ":" + port) ;
	  }

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

  void close()
  {
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
	System.out.println("I/O failed on closing the serverSocket connection to " + host);
      }
  }

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




