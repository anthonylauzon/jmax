
package ircam.jmax.toolkit;

import com.sun.java.swing.undo.*;

/** 
 * An utility class implementing the UndoableData interface.
 * @see UndoableData */
public abstract class AbstractUndoableObject implements UndoableData {
  
  public void beginUpdate(){}
  public void postEdit(UndoableEdit e){}
  public void endUpdate() {}
  public void undo(){}
  public void redo(){}
  public boolean isInGroup() {return false;}
}
