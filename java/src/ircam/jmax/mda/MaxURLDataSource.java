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
}
