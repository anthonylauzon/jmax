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
 * A simple UndoableEdit that can be used when  the action to undo 
 * is related to the initial time of the object.
 * It takes as argument the event that is going to be modified. 
 * @see UndoableAdd */  
public class UndoableMove extends AbstractUndoableEdit {
  
  ScrEvent itsEvent;  

  int undoTime;
  int redoTime;


  /**
   * Constructor. theEvent is the event that is going to be modified. */
  public UndoableMove(ScrEvent theEvent)
  {

    itsEvent = theEvent;

    undoTime = theEvent.getTime();
  }

  public boolean addEdit(UndoableEdit anEdit)
  {
    return false; //these actions don't absorbe other actions
  }

  public String getPresentationName()
  {
    return "event moving";
  }
  
  /**
   * Undo the trasformation */
  public void undo()
  {

    redoTime = itsEvent.getTime();

    itsEvent.move(undoTime);

  }

  /**
   * redo the trasformation */
  public void redo()
  {
    undoTime = itsEvent.getTime();
    
    itsEvent.move(redoTime);

  }

}




