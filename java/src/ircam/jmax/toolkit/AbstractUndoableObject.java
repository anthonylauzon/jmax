
package ircam.jmax.toolkit;

import com.sun.java.swing.undo.*;

/** 
 * An utility class implementing the UndoableData interface.
 * @see UndoableData */
public abstract class AbstractUndoableObject implements UndoableData {
  
  /**
   * Start an undoable section */
  public void beginUpdate()
  {
    inGroup = true;
  }

  /**
   * Add the given UndoableEdit to the undo Buffers */
  public void postEdit(UndoableEdit e){}

  /**
   * Signal that the undoable section ended */
  public void endUpdate() 
  {
    inGroup = false;
  }

  /**
   * undo the last section */
  public void undo(){}

  /** redo the last (undone) section */
  public void redo(){}

  /**
   * Return true if an undoable section have been opened, and not closed yet*/
  public boolean isInGroup() 
  {
    return inGroup;
  }

  //--- Fields
  boolean inGroup = false;
}
