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


