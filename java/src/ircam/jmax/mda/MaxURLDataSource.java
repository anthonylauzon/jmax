package ircam.jmax.mda;

import java.net.*;

class MaxURLDataSource extends MaxDataSource
{
  private URL url;

  MaxURLDataSource(URL url)
  {
    this.url = url;
  }

  public URL getURL()
  {
    return url;
  }

  public String toString()
  {
    return url.toString();
  }

  public String getName()
  {
    String name;

    name = url.toString();
    return name.substring(0, name.lastIndexOf('.'));
  }

  /** Return a boolean that tell if the source correspond
      to an actually, phisically, existing source, or it
      just express an address where a data source can be created.
      */

  public boolean exists()
  {
    // we should know better, may be try to open

    return false;
  }

  /** Return true if we can write to this source.
    All URL are readonly for jmax
    */

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
