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

package ircam.jmax.editors.explode;

import java.awt.*;
import java.util.*;

import javax.swing.undo.*;

/**
 * A simple UndoableEdit that can be used when  the action to undo 
 * is related to one (or more) parameters changing in an event. 
 * It takes as argument the event that is going to be modified. 
 * @see UndoableAdd */  
public class UndoableEventTransformation extends AbstractUndoableEdit {
  
  ScrEvent itsEvent;  

  int undoTime;
  int undoPitch;
  int undoVelocity;
  int undoDuration;
  int undoChannel;

  int redoTime;
  int redoPitch;
  int redoVelocity;
  int redoDuration;
  int redoChannel;

  /**
   * Constructor. theEvent is the event that is going to be modified. */
  public UndoableEventTransformation(ScrEvent theEvent)
  {

    itsEvent = theEvent;

    undoTime = theEvent.getTime();
    undoPitch = theEvent.getPitch();
    undoVelocity = theEvent.getVelocity();
    undoDuration = theEvent.getDuration();
    undoChannel = theEvent.getChannel();
  }

  public boolean addEdit(UndoableEdit anEdit)
  {
    return false; //these actions don't absorbe other actions
  }

  public String getPresentationName()
  {
    return "simple event transformation";
  }
  
  /**
   * Undo the trasformation */
  public void undo()
  {

    redoTime = itsEvent.getTime();
    redoPitch = itsEvent.getPitch();
    redoVelocity = itsEvent.getVelocity();
    redoDuration = itsEvent.getDuration();
    redoChannel = itsEvent.getChannel();

    itsEvent.setTime(undoTime);
    itsEvent.setPitch(undoPitch);
    itsEvent.setVelocity(undoVelocity);
    itsEvent.setDuration(undoDuration);
    itsEvent.setChannel(undoChannel);
  }

  /**
   * redo the trasformation */
  public void redo()
  {
    undoTime = itsEvent.getTime();
    undoPitch = itsEvent.getPitch();
    undoVelocity = itsEvent.getVelocity();
    undoDuration = itsEvent.getDuration();
    undoChannel = itsEvent.getChannel();

    
    itsEvent.setTime(redoTime);
    itsEvent.setPitch(redoPitch);
    itsEvent.setVelocity(redoVelocity);
    itsEvent.setDuration(redoDuration);
    itsEvent.setChannel(redoChannel);
  }

}
