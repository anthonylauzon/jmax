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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

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
