package ircam.jmax.editors.explode;

import java.util.Enumeration;

/**
 * the interface of the classes that handle a selection of objects
 * on a bidimensional plane.
 */
public interface SelectionHandler {

  public abstract void select(Object obj);
  public abstract void selectArea(int x, int y, int w, int h);
  public abstract void deSelect(Object obj);
  public abstract boolean isInSelection(Object obj);
  public abstract Enumeration getSelected();
  public abstract void selectAll();
  public abstract void deselectAll();
}
