package ircam.jmax.toolkit;

/**
 * The interface used by objects that listen to dragging operations,
 * such as the moving tools. The information passed back is limited 
 * to the end of the interaction.
 * Use DynamicDragListener for richer set of informations.
 * @see SelectionMover
 * @see DynamicDragListener 
 */
public interface DragListener {
  
  /**
   * informs that the drag ended at the point x,y
   */
  public abstract void dragEnd(int x, int y);
}



