package ircam.jmax.mda;

import java.net.*;
import java.io.*;

import ircam.jmax.fts.*;

abstract public class MaxDocumentSource
{

  /** This static method produce a MaxDocumentSource of the good
    class starting from an address; current version 
    reconize http:, ftp: and file: as URL, and fts: as
    FTS, and anything else as file */

  static public MaxDocumentSource makeDocumentSource(String address) throws java.net.MalformedURLException
  {
    if (address.startsWith("http:") ||
	address.startsWith("ftp:") ||
	address.startsWith("file:"))
      return new MaxURLDocumentSource(new URL(address));
    else 
      return new MaxFileDocumentSource(new File(address));
  }


  /** This static method produce the correct document source for an URL 
   * object
   */

  public static MaxDocumentSource makeDocumentSource(URL url)
  {
    return new MaxURLDocumentSource(url);
  }


  /** This static method produce a MaxDocumentSource from a File object
   */

  public static MaxDocumentSource makeDocumentSource(File file)
  {
    return new MaxFileDocumentSource(file);
  }  

  /** Return a name that can be used as document name */

  abstract public String getName();

  /** Return a boolean that tell if the source correspond
      to an actually, phisically, existing source, or it
      just express an address where a document source can be created.
      */

  abstract public boolean exists();

  /** Return true if we can write to this source */

  abstract public boolean canWrite();

  /** Return true if we can read from this source */

  abstract public boolean canRead();
}





