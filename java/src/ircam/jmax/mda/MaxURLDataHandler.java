package ircam.jmax.mda; 

import java.net.*;

/** A Data Handler is an object able to load a Max Data instance
 *  from a file data source.
 */

abstract class MaxURLDataHandler extends MaxDataHandler
{
  /** Return true if this Data Handler can load
    from the given address; by default, return true.
    Subclasses should anyway call this method, 
    that can change in the future.
    */

  public boolean canLoadFrom(MaxDataSource source)
  {
    return true;
  }


  /** Return true if this Data Handler can load
    from the given address */

  public MaxData loadInstance(MaxDataSource source)
  {
    MaxURLDataSource urlSource = (MaxURLDataSource) source;
    URL url = urlSource.getURL();
    MaxData obj;

    obj = makeInstance(url);
    
    obj.setDataHandler(this);
    obj.setDataSource(source);

    return obj;
  }

  /** Actually build and return an Max Data from a file */

  abstract protected MaxData makeInstance(URL url);
}
