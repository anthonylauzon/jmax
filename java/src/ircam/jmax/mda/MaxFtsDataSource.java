package ircam.jmax.mda;

import ircam.jmax.fts.*;

public class MaxFtsDataSource extends MaxDataSource
{
  private FtsLocation location;

  public MaxFtsDataSource(FtsLocation location)
  {
    this.location = location;
  }

  public FtsLocation getFtsLocation()
  {
    return location;
  }

  public String toString()
  {
    return location.toString();
  }

  public String getName()
  {
    String name;

    name = location.toString();
    return name.substring(0, name.lastIndexOf('.'));
  }

  /** Return a boolean that tell if the source correspond
      to an actually, phisically, existing source, or it
      just express an address where a data source can be created.
      */

  public boolean exists()
  {
    return location.exists();
  }

  /** Return true if we can write to this source */

  public boolean canWrite()
  {
    return exists();
  }

  /** Return true if we can read from this source */

  public boolean canRead()
  {
    return exists();
  }

}
