package ircam.jmax.mda;

import java.net.*;
import java.io.*;

import ircam.jmax.fts.*;

public class MaxReaderDataSource extends MaxDataSource
{
  Reader reader;

  MaxReaderDataSource(Reader reader)
  {
    this.reader = reader;
  }

  Reader getReader()
  {
    return reader;
  }

  /** Return a name that can be used as document name */

  public String getName()
  {
    return reader.toString();
  }

  public boolean exists()
  {
    return true;
  }

  /** Return true if we can write to this source */

  public boolean canWrite()
  {
    return false;
  }

  /** Return true if we can read from this source */

  public boolean canRead()
  {
    return true;
  }
}
