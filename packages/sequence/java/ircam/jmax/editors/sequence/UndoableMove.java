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

package ircam.jmax.editors.sequence;

import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.track.Event;
import java.awt.*;
import java.util.*;

import javax.swing.undo.*;

/**
 * A simple UndoableEdit that can be used when  the action to undo 
 * is related to the initial time of the object.
 * It takes as argument the event that is going to be modified. 
 * @see UndoableAdd */  
public class UndoableMove extends AbstractUndoableEdit {
    
  Event itsEvent;  
  FtsTrackObject trkObj;
  double undoTime;
  double redoTime;

  /**
   * Constructor. theEvent is the event that is going to be modified. */
  public UndoableMove(TrackEvent theEvent, double newTime)
  {
      try {
	  itsEvent = theEvent.duplicate();
      } catch (Exception ex) {System.err.println("error while cloning event");}
      
      trkObj = ((FtsTrackObject)theEvent.getDataModel());

      undoTime = theEvent.getTime();
      redoTime = newTime;
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
      itsEvent.setTime(redoTime);
      TrackEvent evt = trkObj.getEventLikeThis(itsEvent);
      
      if(evt!=null)
	  evt.move(undoTime);
      else
	  die();
  }

  /**
   * redo the trasformation */
  public void redo()
  {
      itsEvent.setTime(undoTime);
      TrackEvent evt = trkObj.getEventLikeThis(itsEvent);
      if(evt!=null)
	  evt.move(redoTime);
      else
	  die();
  }

}




