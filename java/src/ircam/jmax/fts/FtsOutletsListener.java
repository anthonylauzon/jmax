package ircam.jmax.fts;

/**
 * A specialized listener that listen to changes of the number of outlets
 * of an object.
 */

public interface FtsOutletsListener
{
  public void outletsChanged(int value);
}
