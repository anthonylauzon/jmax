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
}



