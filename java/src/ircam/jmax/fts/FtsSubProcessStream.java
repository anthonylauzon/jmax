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

