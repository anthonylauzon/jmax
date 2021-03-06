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

package ircam.jmax.editors.explode;

import java.awt.*;
import java.util.*;

import javax.swing.undo.*;


/** Undo support: this class knows how to undo an "add note" action. 
 * @see UndoableEventTransformation.*/
class UndoableAdd extends AbstractUndoableEdit {
  ScrEvent itsEvent;
  
  public UndoableAdd(ScrEvent theAddedEvent)
  {
    itsEvent = theAddedEvent;
  }
  
  public boolean addEdit(UndoableEdit anEdit)
  {
    return false; //add actions cannot absorbe other actions
  }

  public String getPresentationName()
  {
    return "add";
  }
  
  public void redo()
  {
    //since a redo can only occur after a corresponding undo,
    // there's no need to copy-construct the deleted event.
      itsEvent.getDataModel().addEvent(itsEvent);
  }  
  
  public void undo()
  {
    itsEvent.getDataModel().deleteEvent(itsEvent);
  }

}

