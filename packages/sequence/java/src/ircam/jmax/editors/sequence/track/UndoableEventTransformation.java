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

package ircam.jmax.editors.sequence.track;

import java.awt.*;
import java.util.*;

import javax.swing.undo.*;

/**
 * A simple UndoableEdit that can be used when  the action to undo 
 * is related to one (or more) parameters changing in an event. 
 * It takes as argument the event that is going to be modified. 
 * This implementation only try to save known properties of the event, that is:
 * <ul>
 *  <li> time </li>
 *  <li> duration </li>
 *  <li> pitch </li>
 *  <li> velocity </li>
 *  <li> channel </li>
 *  <li> ambitus </li>
 * </ul>
 * A better organization is needed...
 * @see UndoableAdd */  
public class UndoableEventTransformation extends AbstractUndoableEdit {
  
  TrackEvent itsEvent;  

    /*int*/double undoTime;
  Integer undoPitch;
  Integer undoVelocity;
  Integer undoDuration;
  Integer undoChannel;
  Integer undoAmbitus;

    /*int*/double redoTime;
  Integer redoPitch;
  Integer redoVelocity;
  Integer redoDuration;
  Integer redoChannel;
  Integer redoAmbitus;

  /**
   * Constructor. theEvent is the event that is going to be modified. */
  public UndoableEventTransformation(TrackEvent theEvent)
  {

    itsEvent = theEvent;

    undoTime = theEvent.getTime();
    undoPitch = (Integer)theEvent.getProperty("pitch");
    undoVelocity = (Integer)theEvent.getProperty("velocity");
    undoDuration = (Integer)theEvent.getProperty("duration");
    undoChannel = (Integer)theEvent.getProperty("channel");
    undoAmbitus = (Integer)theEvent.getProperty("ambitus");
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
    redoPitch = (Integer)itsEvent.getProperty("pitch");
    redoVelocity = (Integer)itsEvent.getProperty("velocity");
    redoDuration = (Integer)itsEvent.getProperty("duration");
    redoChannel = (Integer)itsEvent.getProperty("channel");
    redoAmbitus = (Integer)itsEvent.getProperty("ambitus");

    itsEvent.setTime(undoTime);
    itsEvent.setProperty("pitch", undoPitch);
    itsEvent.setProperty("velocity", undoVelocity);
    itsEvent.setProperty("duration", undoDuration);
    itsEvent.setProperty("channel", undoChannel);
    itsEvent.setProperty("ambitus", undoAmbitus);
  }

  /**
   * redo the trasformation */
  public void redo()
  {
    undoTime = itsEvent.getTime();
    undoPitch = (Integer)itsEvent.getProperty("pitch");
    undoVelocity = (Integer)itsEvent.getProperty("velocity");
    undoDuration = (Integer)itsEvent.getProperty("duration");
    undoChannel = (Integer)itsEvent.getProperty("channel");
    undoAmbitus = (Integer)itsEvent.getProperty("ambitus");

    
    itsEvent.setTime(redoTime);
    itsEvent.setProperty("pitch", redoPitch);
    itsEvent.setProperty("velocity", redoVelocity);
    itsEvent.setProperty("duration", redoDuration);
    itsEvent.setProperty("channel", redoChannel);
    itsEvent.setProperty("ambitus", redoAmbitus);
  }

}



