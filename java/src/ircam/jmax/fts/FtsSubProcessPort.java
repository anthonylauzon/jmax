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

  FtsSubProcessPort(String name)
  {
    super(name);
  }

  void openIOStreams()
  {
    try
      {
	proc = Runtime.getRuntime().exec(path+"/"+ftsName);
	out_stream = proc.getOutputStream();
	in_stream  = proc.getInputStream();

	FtsErrorStreamer.startFtsErrorStreamer(proc.getErrorStream());
      }
    catch (IOException e)
      {
	System.err.println("Don't know about command " + path);
      }    
  }

  void closeIOStreams()
  {
    try
      {
	in_stream.close();
	out_stream.close();
	proc.destroy();
      } 
    catch (IOException e)
      {
	System.err.println("Cannot quit " + path);
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

  // Start just call open

  void start()
  {
    open();
  }
}

