package ircam.jmax.mda;

import java.io.*;

public class MaxFileDocumentSource extends MaxDocumentSource
{
  private File file;

  MaxFileDocumentSource(File file)
  {
    this.file = file;
  }

  public File getFile()
  {
    return file;
  }

  public String toString()
  {
    return file.toString();
  }

  /** Return a name that can be used as document name */

  public String getName()
  {
    return  file.getName();
  }

  /** Return a boolean that tell if the source correspond
      to an actually, phisically, existing source, or it
      just express an address where a document source can be created.
      */

  public boolean exists()
  {
    return file.exists();
  }

  /** Return true if we can write to this source */

  public boolean canWrite()
  {
    return (! file.exists()) || file.canWrite();
  }

  /** Return true if we can read from this source */

  public boolean canRead()
  {
    return file.canRead();
  }

}



