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
package ircam.jmax.editors.explode;

import java.awt.*;
import java.util.*;

import javax.swing.undo.*;


/** 
 * Undo support: this class knows how to undo an "delete note" */
class UndoableDelete extends AbstractUndoableEdit {
  ScrEvent itsEvent;

  public UndoableDelete(ScrEvent theDeletedEvent)
  {
    itsEvent = theDeletedEvent;
  }
  
  public boolean addEdit(UndoableEdit anEdit)
  {
    return false; 
  }
  
  public String getPresentationName()
  {
    return "delete";
  }
  
  public void redo()
  {
    //since a redo can only occur after a corresponding undo,
    // there's no need to copy-construct the deleted event.
    itsEvent.getDataModel().removeEvent(itsEvent);
  }  
  
  public void undo()
  {
    itsEvent.getDataModel().addEvent(itsEvent);
  }

}


