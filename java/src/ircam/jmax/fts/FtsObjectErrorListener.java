package ircam.jmax.fts;

/**
 * A specialized listener that listen to error state changes
 */

public interface FtsObjectErrorListener
{
  public void errorChanged(boolean value);
}
