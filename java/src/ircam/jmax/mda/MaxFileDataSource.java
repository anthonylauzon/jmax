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
}



