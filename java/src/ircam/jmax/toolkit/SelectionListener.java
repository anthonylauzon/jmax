package ircam.jmax.toolkit;

/** Interface of the objects that wants to listen to a selection's content.
 */ 

public interface SelectionListener {
  /** theContent of the selection changed
   */
  public abstract void selectionChanged();
}

