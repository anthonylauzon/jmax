package ircam.jmax.fts;

/**
 * A specialized listener that listen to changes of the number of inlets
 * of an object.
 */

public interface FtsInletsListener
{
  public void inletsChanged(int value);
}
