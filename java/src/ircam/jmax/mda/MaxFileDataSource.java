package ircam.jmax.mda;

import java.io.*;

class MaxFileDataSource extends MaxDataSource
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
}
