package ircam.jmax.mda;

import java.net.*;
import java.io.*;

import ircam.jmax.fts.*;

public class MaxWriterDataSource extends MaxDataSource
{
  Writer writer;


  public MaxWriterDataSource(Writer writer)
  {
    this.writer = writer;
  }

  public Writer getWriter()
  {
    return writer;
  }

  /** Return a name that can be used as document name */

  public String getName()
  {
    return writer.toString();
  }

  public boolean exists()
  {
    return true;
  }

  /** Return true if we can write to this source */

  public boolean canWrite()
  {
    return true;
  }

  /** Return true if we can read from this source */

  public boolean canRead()
  {
    return false;
  }
}
