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

    ExplodeSelection.getSelection().deselectAll();
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



