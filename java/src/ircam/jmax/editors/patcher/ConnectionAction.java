package ircam.jmax.editors.patcher;


/** An interface for actions intended to be mapped on Connections sets.
  Work on selections and on the whole displayList 
  */

public interface ConnectionAction
{
  public void processConnection(ErmesConnection object);
}
