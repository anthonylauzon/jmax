
package ircam.jmax.fts;

import ircam.jmax.toolkit.*;
import javax.swing.undo.*;
import javax.swing.event.*;

/**
 * An fts remote data that offers a built-in undo support.
 * 
 */
public abstract class FtsRemoteUndoableData extends FtsRemoteData implements UndoableData{
  
  public FtsRemoteUndoableData()
  {
    super();

    undoM = new UndoManager();
    undo = new UndoableEditSupport();
    undo.addUndoableEditListener(new UndoableEditListener() {
      public void undoableEditHappened(UndoableEditEvent e)
	{
	  undoM.addEdit(e.getEdit());
	} 
    });
    
  }

  /** NOTE: the begin update - endUpdate methods acts also as a
   * controller for the actions to undo. If no
   * beginUpdate has been call, the modification to the
   * model are not stored in the undo buffers */

  public void beginUpdate()
  {
    undo.beginUpdate();
    inGroup = true;
  }

  public void endUpdate()
  {
    if (!inGroup) return;
    undo.endUpdate();
    inGroup = false;
  }

  public void postEdit(UndoableEdit e)
  {
    undo.postEdit(e);
  }

  public boolean isInGroup()
  {
    return inGroup;
  }

  public void undo()
  {
    undoM.undo();
  }

  public void redo()
  {
    undoM.redo();
  }

  //----- Fields

  UndoableEditSupport undo;
  UndoManager undoM;

  boolean inGroup = false;
}
