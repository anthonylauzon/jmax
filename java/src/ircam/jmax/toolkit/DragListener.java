package ircam.jmax.toolkit;

/**
 * The interface used by objects that listen to dragging operations,
 * such as the moving tools.
 */
public interface DragListener {
  
  /**
   * informs that the drag ended at the point x,y
   */
  public abstract void dragEnd(int x, int y);
}
