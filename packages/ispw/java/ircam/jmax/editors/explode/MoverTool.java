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

import java.awt.*;
import java.util.Enumeration;
import javax.swing.ImageIcon;
import java.awt.event.MouseEvent;

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
  public void dragEnd(int x, int y, MouseEvent ev) 
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

  public void updateStartingPoint(int deltaX, int deltaY)
  {
    startingPoint.x+=deltaX;
    startingPoint.y+=deltaY;
  }

  //------------ Fields
  MouseTracker itsMouseTracker;
  SelectionMover itsSelectionMover;

  public static int HORIZONTAL_MOVEMENT = 1;
  public static int VERTICAL_MOVEMENT = 2;

  Point startingPoint;
}



