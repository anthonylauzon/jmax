//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
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
 * implemented by data models with support for undo.
 * @see ircam.jmax.fts.FtsRemoteUndoableData
 */
public interface UndoableData {

  /**
   * tells the model to start an undo section */
  public abstract void beginUpdate();

  /**
   * posts an undo edit in the buffers */
  public abstract void postEdit(UndoableEdit e);
  
  /**
   * terminates the undo critical section */
  public abstract void endUpdate();
  
  /**
   * ask to undo the section */
  public abstract void undo();

  /**
   * ask to redo the section */
  public abstract void redo();

  /**
   * are we in the midst of an undoable section? */
  public abstract boolean isInGroup();

}



