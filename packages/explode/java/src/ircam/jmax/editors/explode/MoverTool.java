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

import java.awt.*;
import java.util.Enumeration;
import javax.swing.ImageIcon;


/**
 * the tool used to move a selection of events.
 * It uses two UI modules: a MouseTracker to establish the starting point
 * of the moving operation, and a SelectionMover to actually move the
 * objects.
 */
public class MoverTool extends Tool implements PositionListener, DragListener {

  /**
   * constructor.
   */
  public MoverTool(ImageIcon theIcon, int theDirection) 
  {
    super("mover", theIcon);
    
    itsMouseTracker = new MouseTracker(this);
    itsSelectionMover = new ExplodeSelectionMover(this, theDirection);

    startingPoint = new Point();
  }


  /** the default interaction module */
  public InteractionModule getDefaultIM() 
  {
    return itsMouseTracker;
  }

  /**
   * called when this module is "unmounted"
   */
  public void deactivate() 
  {
  }


  /**
   * position listening. It mounts the SelectionMover if the 
   * point choosen is on a selected event. 
   */
  public void positionChoosen(int x, int y, int modifiers) 
  {
    ScrEvent aScrEvent = (ScrEvent) gc.getRenderManager().firstObjectContaining(x, y);
    
    if (aScrEvent != null && ((ExplodeGraphicContext)gc).getSelection().isInSelection(aScrEvent)) 
      {

	startingPoint.setLocation(x, y);

	mountIModule(itsSelectionMover, x, y);

      }
  }


  /**
   * drag listening, called by the SelectionMover UI Module.
   * Moves the selected objects in the new location,
   * and then mount its default UI Module
   */
  public void dragEnd(int x, int y) 
  {
    ScrEvent aEvent;

    int deltaY = y-startingPoint.y;
    int deltaX = x-startingPoint.x;

    ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;

    // starts a serie of undoable transitions
    ((UndoableData) egc.getDataModel()).beginUpdate();

    for (Enumeration e = egc.getSelection().getSelected(); e.hasMoreElements();)
      {
	aEvent = (ScrEvent) e.nextElement();
	
	egc.getAdapter().setX(aEvent, egc.getAdapter().getX(aEvent)+deltaX);
	egc.getAdapter().setY(aEvent, egc.getAdapter().getY(aEvent)+deltaY);
      }

    ((UndoableData) egc.getDataModel()).endUpdate();

    mountIModule(itsMouseTracker);
    gc.getGraphicDestination().repaint();    
  }

  //------------ Fields
  MouseTracker itsMouseTracker;
  SelectionMover itsSelectionMover;

  public static int HORIZONTAL_MOVEMENT = 1;
  public static int VERTICAL_MOVEMENT = 2;

  Point startingPoint;
}



