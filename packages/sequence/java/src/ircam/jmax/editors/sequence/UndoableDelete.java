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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.sequence;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.track.Event;
import java.awt.*;
import java.util.*;

import javax.swing.undo.*;


/** 
 * Undo support: this class knows how to undo an "delete note" */
class UndoableDelete extends AbstractUndoableEdit {
  Event itsEvent;
  FtsTrackObject trkObj;

  public UndoableDelete(TrackEvent theDeletedEvent)
  {
      try {
	  itsEvent = theDeletedEvent.duplicate();
      } catch (Exception ex) {System.err.println("error while cloning event");}
      
      trkObj = ((FtsTrackObject)theDeletedEvent.getDataModel());
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
      TrackEvent evt = trkObj.getEventLikeThis(itsEvent);
      if(evt!=null)
	  trkObj.removeEvent(evt);
      else
	  die();
  }  
  
  public void undo()
  {
      SequenceSelection.getCurrent().deselectAll();
      trkObj.requestEventCreation((float)itsEvent.getTime(), 
				  itsEvent.getValue().getValueInfo().getName(), 
				  itsEvent.getValue().getPropertyCount(), 
				  itsEvent.getValue().getPropertyValues());
  }
}




