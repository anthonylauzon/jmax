package ircam.jmax.toolkit;

/**
 * The interface of the graphic clients of a XORHandler object.
 * Such clients are informed when an erase or a draw operation is
 * needed. For instruction,
 * @see XORHandler
 */ 
public interface XORPainter {

  /**
   * Undo the last drawing */
  public abstract void XORErase();
  /**
   * make a new draw in the new position */
  public abstract void XORDraw(int dx, int dy);
}
