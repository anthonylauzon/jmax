package  ircam.jmax.fts;

/** 
 * An FtsUpdateGroupListener is informed when 
 * an update group start, and when it finish.
 * 
 */

public interface FtsUpdateGroupListener
{
  abstract public void updateGroupStart();
  abstract public void updateGroupEnd();
}
