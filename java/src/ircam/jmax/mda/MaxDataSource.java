package ircam.jmax.mda;

import java.net.*;
import java.io.*;

import ircam.jmax.fts.*;

abstract public class MaxDataSource
{

  /** This static method produce a MaxDataSource of the good
    class starting from an address; current version 
    reconize http:, ftp: and file: as URL, and fts: as
    FTS, and anything else as file */

  static public MaxDataSource makeDataSource(String address) throws java.net.MalformedURLException
  {
    
    if (address.startsWith("http:") ||
	address.startsWith("ftp:") ||
	address.startsWith("file:"))
      return new MaxURLDataSource(new URL(address));
    else if (address.startsWith("fts:"))
      return new MaxFtsDataSource(new FtsLocation(address));
    else 
      return new MaxFileDataSource(new File(address));
  }


  /** This static method produce the correct data source for an URL 
   * object
   */

  public static MaxDataSource makeDataSource(URL url)
  {
    return new MaxURLDataSource(url);
  }

  /** This static method produce a MaxDataSource from a FtsLocation object
   *
   */

  public static MaxDataSource makeDataSource(FtsLocation location)
  {
    return new MaxFtsDataSource(location);
  }

  /** This static method produce a MaxDataSource from a File object
   */

  public static MaxDataSource makeDataSource(File file)
  {
    return new MaxFileDataSource(file);
  }  

  /** Return a name that can be used as document name */

  abstract public String getName();

  /** Return a boolean that tell if the source correspond
      to an actually, phisically, existing source, or it
      just express an address where a data source can be created.
      */

  abstract public boolean exists();

  /** Return true if we can write to this source */

  abstract public boolean canWrite();

  /** Return true if we can read from this source */

  abstract public boolean canRead();
}





