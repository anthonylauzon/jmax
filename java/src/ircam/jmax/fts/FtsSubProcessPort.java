package ircam.jmax.fts;

import java.io.*;
import java.net.*;

/**
 * The subProcess connection.
 * Implement a specialed connection using pipes
 * for communicating with FTS; start FTS as a subprocess.
 */

class FtsSubProcessPort extends FtsPort
{
  String path = ".";
  String ftsName = "fts";
  Process proc;
  InputStream in_stream = null;
  OutputStream out_stream = null;

  FtsSubProcessPort(String name)
  {
    super(name);
  }

  void open()
  {
    try
      {
	proc = Runtime.getRuntime().exec(path+"/"+ftsName);
	out_stream = proc.getOutputStream();
	in_stream  = proc.getInputStream();

	FtsErrorStreamer.startFtsErrorStreamer(proc.getErrorStream(), server);
      }
    catch (IOException e)
      {
	System.out.println("Don't know about command " + path);
      }    

    super.open();
  }

  void doClose()
  {
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

  // Start just call open

  void start()
  {
    open();
  }

  /** Method to send a char; since we can use datagram sockets or other
    means I/O is not necessarly done thru streams */

  protected void write(int data) throws java.io.IOException
  {
    out_stream.write(data);
  }

  /** Method to receive a char; since we can use datagram sockets or other
    means I/O is not necessarly done thru streams */

  protected int read() throws java.io.IOException
  {
    return in_stream.read();
  }

  /** Method to Ask for an explicit output flush ; since we
    can use datagram sockets or other means I/O is not necessarly done
    thru streams */

  void flush() throws java.io.IOException
  {
    out_stream.flush();
  }
}

