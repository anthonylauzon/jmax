package ircam.jmax.fts;

import java.io.*;
import java.net.*;

/**
 * The socket connection.
 * Implement a specialed connection using a socket (server side)
 * for communicating with FTS; start FTS thru a rsh command,
 * so it will work only on unix machines and the like.
 */

class FtsSocketServerPort extends FtsPort
{
  String host;
  int port;
  Socket socket;
  ServerSocket serverSocket;
  String path = ".";
  String ftsName = "fts";
  Process proc;

  FtsSocketServerPort(String host)
  {
    super(host);
    this.host = host;

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
  }

  void openIOStreams()
  {
    String command;

    try
      {
	command = ("rsh " + host + " " + path + "/" + ftsName
		   + " socket " + InetAddress.getLocalHost().getHostAddress() + ":" + port) ;
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
	in_stream  = socket.getInputStream();

	FtsErrorStreamer.startFtsErrorStreamer(proc.getErrorStream());
      }
    catch (IOException e)
      {
	System.out.println("Couldn't get I/O for the socket ");
	return;
      }    
  }

  void closeIOStreams()
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

  void setParameter(String property, Object value)
  {
    if (property.equals("ftsdir") && (value instanceof String))
      {
	path = (String) value;
      }
    else if (property.equals("ftsname") && (value instanceof String))
      {
	ftsName = (String) value;
      }
  }

  void start()
  {
    open();
  }
}




