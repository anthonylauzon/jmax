package ircam.jmax.fts;

/** This interface define a listener of the changes
  inside a patcher; for now, only edit changes are reported.
  In the short future, this class will completely substitute
  properties watchers.
  */

public interface FtsPatcherListener
{
  public void objectAdded(FtsPatcherData data, FtsObject object);
  public void objectRemoved(FtsPatcherData data, FtsObject object);

  public void connectionAdded(FtsPatcherData data, FtsConnection connection);
  public void connectionRemoved(FtsPatcherData data, FtsConnection connection);

  public void patcherChanged(FtsPatcherData data);
}
