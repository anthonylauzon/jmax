package ircam.jmax.editors.explode;

/**
 * Interface of the objects that wants to listen to a geometric
 * selection in a plane
 */ 
public interface SelectionListener {
  /**
   * a selection have been choosen
   */
  public abstract void selectionChoosen(int x, int y, int w, int h);
}

