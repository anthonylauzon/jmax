package ircam.jmax.toolkit;

import java.util.Enumeration;

/**
 * the interface implemented by the handlers of generic selections of objects.
 * It makes no assumption on the kind of objects and on the model of data.
 */
public interface SelectionHandler {

  /** select the given object 
   */
  public abstract void select(Object obj);
  

  /** remove the given object from the selection
   */
  public abstract void deSelect(Object obj);


  /** returns true if the object is currently selected
   */
  public abstract boolean isInSelection(Object obj);
  

  /** returns an enumeration of all the selected objects
   */
  public abstract Enumeration getSelected();
  

  /** returns the number of objects in the selection
   */
  public abstract int size();


  /** selects all the objects. This method actually
   * depend on the concrete model used.
   */
  public abstract void selectAll();

  /** deselects all the objects currently selected
   */
  public abstract void deselectAll();


  /** ask to be informed when the selection's content changes
   */
  public abstract void addSelectionListener(SelectionListener theListener);


  /** remove the given listener
   */
  public abstract void removeSelectionListener(SelectionListener theListener);

}









