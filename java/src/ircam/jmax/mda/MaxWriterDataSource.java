package ircam.jmax.mda;

import java.net.*;
import java.io.*;

import ircam.jmax.fts.*;

public class MaxWriterDataSource extends MaxDataSource
{
  Writer Writer;

  public MaxWriterDataSource(Writer Writer)
  {
    this.Writer = Writer;
  }

  public Writer getWriter()
  {
    return Writer;
  }

  /** Return a name that can be used as document name */

  public String getName()
  {
    return Writer.toString();
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
