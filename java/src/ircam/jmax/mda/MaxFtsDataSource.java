package ircam.jmax.mda;

import ircam.jmax.fts.*;

class MaxFtsDataSource extends MaxDataSource
{
  private FtsLocator locator;

  MaxFtsDataSource(FtsLocator locator)
  {
    this.locator = locator;
  }

  public FtsLocator getFtsLocator()
  {
    return locator;
  }

  public String toString()
  {
    return locator.toString();
  }
}
