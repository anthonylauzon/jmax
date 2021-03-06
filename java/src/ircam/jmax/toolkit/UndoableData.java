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

// import javax.swing.undo.*;
import javax.swing.undo.UndoableEdit;

/**
 * implemented by data models with support for undo.
 * @see ircam.jmax.fts.FtsRemoteUndoableData
 */
public interface UndoableData {

  /**
   * tells the model to start an undo section */
  public abstract void beginUpdate();
  
  /**
  * tells the model to start an undo section with the name of action */
  public abstract void beginUpdate(String type);

  /**
   * posts an undo edit in the buffers */
  public abstract void postEdit(UndoableEdit e);
  
  /**
   * terminates the undo critical section */
  public abstract void endUpdate();
  
  /**
    * terminates the undo critical section with name of action */
  public abstract void endUpdate(String type);
  
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



