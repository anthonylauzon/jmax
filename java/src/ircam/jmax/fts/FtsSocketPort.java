package ircam.jmax.fts;

import java.io.*;
import java.net.*;

/**
 * The socket connection.
 * Implement a specialed connection using a socket (client side)
 * for communicating with FTS
 */

class FtsSocketPort extends FtsPort
{
  String host;
  int port;
  Socket socket;

  FtsSocketPort(String host, int port)
  {
    super(host+":"+port);
    this.host = host;
    this.port = port;

    open(); // the socket open immediately the connection
  }

  void openIOStreams()
  {
    try
      {
	socket = new Socket(host, port);
	out_stream = socket.getOutputStream();
	in_stream  = socket.getInputStream();
      }
    catch (UnknownHostException e)
      {
	System.out.println("Don't know about host " + host + ":" + port);
      }
    catch (IOException e)
      {
	System.out.println("Couldn't get I/O for the connection to " + host + ":" + port);
      }    
  }

  // New behaviour: stop the server when close connection: ok, because we
  // say we are now monoclient .

  void closeIOStreams()
  {
    try
      {
	in_stream.close();
	out_stream.close();
	socket.close();
      } 
    catch (IOException e)
      {
	System.out.println("I/O failed on closing the connection to " + host + ":" + port);
      }

  }

  void setParameter(String property, Object value)
  {
    // no parameter to set.
  }

  void start()
  {
  }
}




