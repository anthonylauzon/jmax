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

import ircam.jmax.toolkit.*;

import java.awt.event.*;
import java.awt.*;
import javax.swing.ImageIcon;
import javax.swing.undo.*;

/**
 * The tool used to add an event in the score.
 * It uses just one user-interaction module:
 * a mouse tracker to choose the position.
 */ 
public class ScrAddingTool extends Tool implements PositionListener {

  /**
   * Constructor. It needs to know the graphic source of events,
   * the Adapter, the model. All these informations are in the
   * given graphic context.
   */
  public ScrAddingTool(ImageIcon theImageIcon) 
  {
    super("adder", theImageIcon);

    itsMouseTracker = new VerboseMouseTracker(this);
  }


  /**
   * the default interaction module for this tool
   */
  public InteractionModule getDefaultIM() 
  {
    return itsMouseTracker;
  }


  /**
   * called when this tool is "unmounted"
   */
  public void deactivate() 
  {
  }

  
  /**
   *PositionListener interface
   */
  public void positionChoosen(int x, int y, int modifiers) 
  {
    ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;
    ScrEvent aEvent = new ScrEvent();

    egc.getSelection().deselectAll();
    egc.getAdapter().setX(aEvent, x);
    egc.getAdapter().setY(aEvent, y);

    // starts an undoable transition
    ((UndoableData) egc.getDataModel()).beginUpdate();

    egc.getDataModel().addEvent(aEvent);

    // ends the undoable transition
    ((UndoableData) egc.getDataModel()).endUpdate();

    
  } 
  
  //-------------- Fields

  MouseTracker itsMouseTracker;
}



