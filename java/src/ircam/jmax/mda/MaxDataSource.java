package ircam.jmax.mda;

import java.net.*;
import java.io.*;

import ircam.jmax.fts.*;

class MaxDataSource
{

  /** This static method produce a MaxDataSource of the good
    class starting from an address; current version 
    reconize http:, ftp: and file: as URL, and fts: as
    FTS, and anything else as file */

  static MaxDataSource makeDataSource(String address) throws java.net.MalformedURLException
  {
    if (address.startsWith("http:") ||
	address.startsWith("ftp:") ||
	address.startsWith("file:"))
      return new MaxURLDataSource(new URL(address));
    else if (address.startsWith("fts:"))
      return new MaxFtsDataSource(new FtsLocator(address));
    else 
      return new MaxFileDataSource(new File(address));
  }


  /** This static method produce the correct data source for an URL 
   * object
   */

  static MaxDataSource makeDataSource(URL url)
  {
    return new MaxURLDataSource(url);
  }

  /** This static method produce a MaxDataSource from a FtsLocator object
   *
   */

  static MaxDataSource makeDataSource(FtsLocator locator)
  {
    return new MaxFtsDataSource(locator);
  }

  /** This static method produce a MaxDataSource from a File object
   */

  static MaxDataSource makeDataSource(File file)
  {
    return new MaxFileDataSource(file);
  }
}


