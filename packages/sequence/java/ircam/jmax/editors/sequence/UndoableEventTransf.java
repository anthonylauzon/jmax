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

import java.awt.*;
import java.util.*;

import javax.swing.undo.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.track.Event;

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
public class UndoableEventTransf extends AbstractUndoableEdit {
  
  Event itsEvent;    
  FtsTrackObject trkObj;
  String propName;
  Object undoProp;
  Object redoProp;
  int numberOfProperty;
  
  String[] propNames;
  Object[] undoProps;
  Object[] redoProps;
  
  /**
   * Constructor. theEvent is the event that is going to be modified. */
  public UndoableEventTransf(TrackEvent theEvent, String propName, Object newValue)
  {    
    try {
      itsEvent = theEvent.duplicate();
    } catch (Exception ex) {System.err.println("error while cloning event");}
      
    trkObj = ((FtsTrackObject)theEvent.getDataModel());
    
    this.propName  = propName;
    undoProp  = theEvent.getProperty(propName);
    redoProp  = newValue;
    numberOfProperty = 1;
  }
  
  public UndoableEventTransf(TrackEvent theEvent, int numOfProp, String[] undoPropNames, Object[] oldUndoVals, Object[] newUndoVals)
  {
    try {
      itsEvent = theEvent.duplicate();
    } catch (Exception ex) {System.err.println("error while cloning event");}
    
    trkObj = ((FtsTrackObject)theEvent.getDataModel());
    
    numberOfProperty = numOfProp;
    if( numberOfProperty > 1)
    {
      propNames = new String[numberOfProperty];
      undoProps = new Object[numberOfProperty];
      redoProps = new Object[numberOfProperty];
      
      for(int i = 0; i< numberOfProperty; i++)
      {
        propNames[i] = undoPropNames[i];
        undoProps[i] = oldUndoVals[i];
        redoProps[i] = newUndoVals[i];
      }
    }
    else if(numberOfProperty == 1)
    {
      this.propName  = undoPropNames[0];
      undoProp  = oldUndoVals[0];
      redoProp  = newUndoVals[0];
    }
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
    if(numberOfProperty == 1)
    {     
      itsEvent.setProperty(propName, redoProp);
      TrackEvent evt = trkObj.getEventLikeThis(itsEvent);
        
      if(evt!=null)
        evt.setProperty(propName, undoProp);
      else
        die();
    }
    else 
    {
      for(int i = 0; i< numberOfProperty; i++)
        itsEvent.setProperty(propNames[i], redoProps[i]);
      
      TrackEvent evt = trkObj.getEventLikeThis(itsEvent);
      
      if(evt!=null)
      {
        for(int i = 0; i< numberOfProperty; i++)
          evt.setProperty(propNames[i], undoProps[i]);
      }
      else
        die();
    }
  }

  /**
   * redo the trasformation */
  public void redo()
  {
    if(numberOfProperty == 1)
    {
      itsEvent.setProperty(propName, undoProp);
      TrackEvent evt = trkObj.getEventLikeThis(itsEvent);
      
      if(evt!=null)
        evt.setProperty(propName, redoProp);
      else
        die();
    }
    else
    {
      for(int i = 0; i< numberOfProperty; i++)
        itsEvent.setProperty(propNames[i], undoProps[i]);
      
      TrackEvent evt = trkObj.getEventLikeThis(itsEvent);
      if(evt!=null)
      {
        for(int i = 0; i< numberOfProperty; i++)
          evt.setProperty(propNames[i], redoProps[i]);
      }
      else
        die();
    }
  }
}



