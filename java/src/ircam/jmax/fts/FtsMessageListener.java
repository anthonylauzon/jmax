package ircam.jmax.fts;

/**
 * A specialized listener that listen to message changes
 * in a message box
 */

public interface FtsMessageListener
{
  public void messageChanged(String message);
}
