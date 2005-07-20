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

import javax.swing.*;
import javax.swing.undo.*;


/** Undo support: this class knows how to undo an "add note" action. 
 * @see UndoableEventTransformation.*/
class UndoableAdd extends AbstractUndoableEdit {
  Event itsEvent;
  FtsTrackObject trkObj;
	boolean isMarker = false;
	
  public UndoableAdd(TrackEvent theAddedEvent)
  {
    try {
      itsEvent = theAddedEvent.duplicate();
    } catch (Exception ex) {
      System.err.println("error while cloning event");
      Thread.dumpStack();
    }
      
		isMarker = ( itsEvent.getValue() instanceof MarkerValue);
    trkObj = ((FtsTrackObject)theAddedEvent.getDataModel());
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
    SwingUtilities.invokeLater(new Runnable() {
	    public void run()
      { 
        SequenceSelection.getCurrent().deselectAll();
        if(isMarker)
          trkObj.appendBar(null);
        else
          trkObj.requestEventCreation((float)itsEvent.getTime(), 
                                      itsEvent.getValue().getValueInfo().getName(), 
                                      itsEvent.getValue().getDefinedPropertyCount()*2,
                                      itsEvent.getValue().getDefinedPropertyNamesAndValues());
      }  
    });
  }
  public void undo()
  { 
    SwingUtilities.invokeLater(new Runnable() {
	    public void run()
      { 
        TrackEvent evt = trkObj.getEventLikeThis(itsEvent);
        if(evt!=null)
          trkObj.deleteEvent(evt);
        else
          die();
      }
    });
  }
}
