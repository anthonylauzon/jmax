package ircam.jmax.toolkit;

/**
 * Interface of the objects that wants to listen to a geometric
 * selection in a plane
 */ 
public interface GraphicSelectionListener {
  
  /** a rectangular selection have been choosen */
  public abstract void selectionChoosen(int x, int y, int w, int h);

  /** a point have been choosen */
  public abstract void selectionPointChoosen(int x, int y, int modifiers);
}


