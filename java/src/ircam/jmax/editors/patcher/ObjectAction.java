package ircam.jmax.editors.patcher;

import ircam.jmax.editors.patcher.objects.*;

/** An interface for actions intended to be mapped on objects sets.
  Work on selections and on the whole displayList 
  */

public interface ObjectAction
{
  public void processObject(ErmesObject object);
}
