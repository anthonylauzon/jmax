 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.fts;

import ircam.fts.client.*;
import ircam.jmax.toolkit.*;

// import javax.swing.event.*;
import javax.swing.event.UndoableEditListener;
import javax.swing.event.UndoableEditEvent;

// import javax.swing.undo.*;
import javax.swing.undo.UndoableEdit;
import javax.swing.undo.UndoableEditSupport;
import javax.swing.undo.UndoManager;

import java.io.*;

/**
 * An fts remote data that offers a built-in undo support.
 * 
 */
public class FtsUndoableObject extends FtsGraphicObject implements UndoableData{
  
  public FtsUndoableObject(FtsServer server, FtsObject parent, int id, String className, FtsAtom[] args, int offset, int length)
  {
    super(server, parent, id, className, args, offset, length);
    init();
  }

  public FtsUndoableObject(FtsServer server, FtsObject parent, int id, String className, String description)
  {
    super(server, parent, id, className, description);
    init();
  }

  public FtsUndoableObject(FtsServer server, FtsObject parent, int id)
  {
    super(server, parent, id);
    init();
  }

  public  FtsUndoableObject(FtsServer server, FtsObject parent, FtsSymbol className) throws IOException
  {
    super(server, parent, className);
  }

  void init()
  {
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

  public void clearAllUndoRedo()
  {
    undoM.discardAllEdits();
  }

  //----- Fields

  private transient UndoableEditSupport undo;
  transient UndoManager undoM;

  transient   boolean inGroup = false;
}




