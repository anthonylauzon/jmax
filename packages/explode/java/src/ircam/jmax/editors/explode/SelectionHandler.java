package ircam.jmax.editors.explode;

import java.util.Enumeration;

/**
 * the interface implemented by the handlers of generic selections of objects
 */
public interface SelectionHandler {

  /**
   * select the given object 
   */
  public abstract void select(Object obj);
  
  /**
   * remove the given object from the selection
   */
  public abstract void deSelect(Object obj);

  /**
   * returns true if the object is currently selected
   */
  public abstract boolean isInSelection(Object obj);
  
  /**
   * returns an enumeration of all the selected objects
   */
  public abstract Enumeration getSelected();
  
  /**
   * selects all the objects. This method can be overriden to actually
   * select all the objects in a model.
   */
  public abstract void selectAll();

  /**
   * deselects all the objects currently selected
   */
  public abstract void deselectAll();


}
