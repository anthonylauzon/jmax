//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.fts;

import java.io.*;
import java.net.*;

/**
 * The subProcess connection.
 * Implement a specialed connection using pipes
 * for communicating with FTS; start FTS as a subprocess.
 */

class FtsSubProcessStream extends FtsStream
{
  String path;
  String ftsName;
  Process proc;
  InputStream in_stream = null;
  OutputStream out_stream = null;

  FtsSubProcessStream(String path, String ftsName)
  {
    super("fts");
    this.path = path;
    this.ftsName = ftsName;

    try
      {
	proc = Runtime.getRuntime().exec(path+"/"+ftsName);
	out_stream = proc.getOutputStream();
	in_stream  = proc.getInputStream();

	FtsErrorStreamer.startFtsErrorStreamer(proc.getErrorStream());
      }
    catch (IOException e)
      {
	System.out.println("Don't know about command " + path);
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
	proc.destroy();
      } 
    catch (IOException e)
      {
	System.out.println("Cannot quit " + path);
      }
  }

  /** Check if the connection is open */

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

  protected int read() throws java.io.IOException, FtsQuittedException
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

