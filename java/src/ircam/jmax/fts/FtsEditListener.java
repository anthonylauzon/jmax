package ircam.jmax.fts;

/**
  This interface define a listener of the changes
  at the global level; for now, only objectRemoved is actually
  called.
  */

public interface FtsEditListener
{
  public void objectAdded(FtsObject object);
  public void objectRemoved(FtsObject object);

  public void connectionAdded(FtsConnection connection);
  public void connectionRemoved(FtsConnection connection);
}
