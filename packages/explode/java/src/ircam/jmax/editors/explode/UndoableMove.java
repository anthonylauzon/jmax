//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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




