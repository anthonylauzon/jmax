package ircam.jmax.editors.explode;

/**
 * Interface of the objects that wants to listen to the (unique) selection
 * in the system.
 */ 
public interface SelectionListener {
  /**
   * a selection have been choosen
   */
  public abstract void selectionChanged();
}

