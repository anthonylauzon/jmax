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
}
