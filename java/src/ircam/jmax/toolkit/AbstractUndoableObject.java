 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.toolkit;

//import javax.swing.undo.*;
import javax.swing.undo.UndoableEdit;

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
  
  String actionType = null;
  public void beginUpdate(String type)
  {
    inGroup = true;
    actionType = type;
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
  
  public void endUpdate(String type) 
  {
    if(actionType == null || ((actionType != null) && actionType.equals(type)))
    {
      inGroup = false;
      actionType = null;
    }  
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
