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

public class FtsSocketConnection extends FtsServerConnection {

  private static final int DEFAULT_PORT = 2023;
  private static final int DEFAULT_CONNECT_TIMEOUT = 30;

  /**
   *
   * Constructs a new FtsSocketConnection to the specified host and port.
   *
   * Tries to connect to FTS. Retries until connection is made or timed'out.
   *
   * @param hostname the host name
   * @param port the port number
   * @param connectTimeout the timeout value in seconds
   * @throws IOException if an I/O error occurs when creating the socket
   * @throws FtsClientException if a timeout occured when trying to connect
   * @throws UnknownHostException if the IP address of the host to which the connection is made could not be determined
   */
  public FtsSocketConnection( String hostname, int port, int connectTimeout) throws FtsClientException, IOException, UnknownHostException
  {
    do
      {
	try
	  {
	    socket = new Socket( hostname, port);
	  }
	catch( IOException e)
	  {
	  }

	if (socket != null)
	  break;

	try
	  {
	    Thread.sleep( 1000);
	  }
	catch (InterruptedException e)
	  {
	    throw new FtsClientException( "Connection interrupted");
	  }

	connectTimeout--;
      }
    while (connectTimeout > 0);

    if ( connectTimeout <= 0)
      throw new FtsClientException( "Cannot connect");

    in = socket.getInputStream();
    out = socket.getOutputStream();
  }

  public FtsSocketConnection( String hostname, int port) throws FtsClientException, IOException, UnknownHostException
  {
    this( hostname, port, DEFAULT_CONNECT_TIMEOUT);
  }

  public FtsSocketConnection( String hostname) throws FtsClientException, IOException, UnknownHostException
  {
    this( hostname, DEFAULT_PORT, DEFAULT_CONNECT_TIMEOUT);
  }  

  public FtsSocketConnection() throws FtsClientException, IOException, UnknownHostException
  {
    this( "127.0.0.1", DEFAULT_PORT, DEFAULT_CONNECT_TIMEOUT);
  }  

  public void close() throws IOException
  {
    socket.close();
    in = null;
    out = null;
  }

  public int read( byte[] b, int off, int len) throws IOException
  {
    return in.read( b, off, len);
  }

  public void write( byte[] b, int off, int len) throws IOException
  {
    out.write( b, off, len);
  }

  private Socket socket;
  private InputStream in;
  private OutputStream out;
}

