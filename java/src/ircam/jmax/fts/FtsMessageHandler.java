package ircam.jmax.fts;

/**
 * Handle an FTS Message. This interface should be implemented
 * in order to install an object as handler of a direct message
 * coming from FTS.
 * 
 * @author mdc
 * @see FtsObject#installMessageHandler
 * @see FtsObject#removeMessageHandler
 */
 
public interface FtsMessageHandler
{
  /**
   * Called by the application layer when an FTS message is received.
   * 
   * @param msg the FtsMessage
   * @see FtsMessage
   */

  void handleMessage(FtsMessage msg);
}
