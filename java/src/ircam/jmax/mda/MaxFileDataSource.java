package ircam.jmax.mda;

import java.io.*;

public class MaxFileDataSource extends MaxDataSource
{
  private File file;

  MaxFileDataSource(File file)
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
    String name;

    name = file.getName();
    return name.substring(0, name.lastIndexOf('.'));
  }

  /** Return a boolean that tell if the source correspond
      to an actually, phisically, existing source, or it
      just express an address where a data source can be created.
      */

  public boolean exists()
  {
    return file.exists();
  }

  /** Return true if we can write to this source */

  public boolean canWrite()
  {
    return file.canWrite();
  }

  /** Return true if we can read from this source */

  public boolean canRead()
  {
    return file.canRead();
  }

}



