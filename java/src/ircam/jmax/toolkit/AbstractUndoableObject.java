//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

package ircam.jmax.toolkit;

import javax.swing.undo.*;

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
