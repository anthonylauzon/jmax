package ircam.jmax.editors.explode;

/**
 * the interface of the graphic clients of a XORHandler object.
 * Such clients are informed when an erase or a draw operation is
 * needed (see also XORHandler class and the associated instruction).
 */ 
public interface XORPainter {

  public abstract void XORErase();
  public abstract void XORDeltaDraw(int dx, int dy);
}
