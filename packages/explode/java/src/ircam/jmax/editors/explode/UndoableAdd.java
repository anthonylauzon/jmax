  
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
    itsEvent.getDataModel().removeEvent(itsEvent);
  }

}

