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

package ircam.jmax.editors.sequence.tools;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.track.*;
import java.awt.event.*;
import java.awt.*;
import javax.swing.*;
import javax.swing.undo.*;
import java.util.*;

/**
 * The tool used to add an event in a track.
 * It uses just one user-interaction module:
 * a mouse tracker to choose the position.
 * The type of object to add is given by the track's type.
 */ 
public class AdderTool extends Tool implements PositionListener {

  /**
   * Constructor with an Icon.
   */
  public AdderTool(ImageIcon theImageIcon) 
  {
    super("adder", theImageIcon);
    
    itsMidiMouseTracker = new MidiMouseTracker(this);
  }

  /**
   * the default interaction module for this tool
   */
  public InteractionModule getDefaultIM() 
  {
    return itsMidiMouseTracker;
  }
  
  /**
   * called when this tool is "unmounted"
   */
  public void deactivate() 
  {
  }
  
  int remember_x; 
  int remember_y;
  /**
   *PositionListener interface
   */
  public void positionChoosen(int x, int y, int modifiers) 
  {
    SequenceGraphicContext egc = (SequenceGraphicContext) gc;
    
    egc.getTrack().setProperty("selected", Boolean.TRUE);
  
    if(egc.getDataModel().isLocked()) return;
    
    // starts an undoable transition	
    ((UndoableData) egc.getDataModel()).beginUpdate();
    //endUpdate is called in addEvents in dataModel
    
    //with Shift add to selection
    if((modifiers & InputEvent.SHIFT_MASK) == 0) egc.getSelection().deselectAll();
    
    ValueInfo info = egc.getTrack().getTrackDataModel().getType();
    addEvent(x, y, (EventValue) info.newInstance());
  }
  
  void addEvent(int x, int y, EventValue value)
  {
    UtilTrackEvent aEvent = new UtilTrackEvent(value);
    SequenceGraphicContext egc = (SequenceGraphicContext) gc;
      
    egc.getAdapter().setX(aEvent, x);
      
    egc.getAdapter().setY(aEvent, y);
    
    egc.getTrack().getFtsTrack().requestEventCreation((float)aEvent.getTime(), 
						      value.getValueInfo().getName(), 
						      value.getPropertyCount(),
						      value.getPropertyValues());
  }
  
  //-------------- Fields
  MidiMouseTracker itsMidiMouseTracker;
}











