package ircam.jmax.toolkit;

/** Interface of the objects that wants to listen to a selection's content.
 * This interface should probably be enriched (see the swing List Listeners)  
 */ 

public interface SelectionListener {
  /** theContent of the selection changed
   */
  public abstract void selectionChanged();
}

