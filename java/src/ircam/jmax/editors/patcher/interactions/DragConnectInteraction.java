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

package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** Make a connection from an outlet to an inlet */


class DragConnectInteraction extends Interaction
{
  boolean dragged = false;
  Point dragStart = new Point();
  GraphicObject src;
  int outlet;
  GraphicObject dst;
  int inlet;

  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true); // need the drag
    filter.setFollowingInOutletLocations(true);
    filter.setFollowingInletLocations(true);
    filter.setFollowingAlsoConnectionLocations(false);
    filter.setAutoScrolling(true);
  }

  // Utility method

  private void doConnection(ErmesSketchPad editor, GraphicObject src, int outlet, GraphicObject dst, int inlet)
  {
    if (src == dst)
      {
	editor.showMessage("Cannot connect an object to itself !");
	return;
      }

    editor.resetUndoRedo();

    editor.addingConnection();
    editor.getFtsPatcher().requestAddConnection(src.getFtsObject(), outlet, dst.getFtsObject(), inlet);
  }

  boolean destinationChoosen = false;

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack) && Squeack.onOutlet(squeack))
      {
	ErmesSelection.patcherSelection.redraw();
	ErmesSelection.patcherSelection.deselectAll();

	src    = (GraphicObject) area.getTarget();
	outlet = area.getNumber();

	dragStart.x = src.getOutletAnchorX(outlet);
	dragStart.y = src.getOutletAnchorY(outlet);
	//editor.resetHighlightedOutlet();
	editor.setConnectingObject(src);
	dragged = false;
      }
    else 
      if(Squeack.isUp(squeack)){
	if (dragged)
	  {
	    // Dragged: 
	    //  do the connection if we have a destination
	    if (destinationChoosen)
	      {
		editor.resetHighlightedInlet(); 
		doConnection(editor, src, outlet, dst, inlet);
	      }

	    // clean up
	    if((!Squeack.isShift(squeack))||(!destinationChoosen)){
	      editor.resetHighlightedOutlet();//??

	      editor.getDisplayList().noDrag();
	      editor.getDisplayList().redrawDragLine();
	      editor.setConnectingObject(null);
	      destinationChoosen = false;
	    
	      editor.endInteraction();
	    }
	    else if(Squeack.isShift(squeack)){//shift pressed: moveReverseInteraction setting to do multiconnect 
	      editor.getEngine().setInteraction(Interactions.moveConnectInteraction);
	      ((MoveConnectInteraction)editor.getEngine().getCurrentInteraction()).setSrc(src, outlet);
	      editor.getEngine().getCurrentInteraction().gotSqueack(editor, squeack, area, mouse, oldMouse);
	    }
	  }
	else
	  {
	    // Not dragged, start a move connection
	    editor.getEngine().setInteraction(Interactions.moveConnectInteraction);
	    editor.getEngine().getCurrentInteraction().gotSqueack(editor, squeack, area, mouse, oldMouse);
	  }
      }
    else if (Squeack.isDrag(squeack) && Squeack.onInlet(squeack))
      {
	dragged = true;

	if ((! destinationChoosen) || dst != (GraphicObject) area.getTarget() || inlet != area.getNumber())
	  {
	    if(!editor.getDisplayList().thisConnectionExist(src, outlet, 
							    (GraphicObject) area.getTarget(), area.getNumber()))
	      {
		dst   = (GraphicObject) area.getTarget();
		inlet = area.getNumber();
		editor.setHighlightedInlet(dst, inlet);
		destinationChoosen = true;
		editor.getDisplayList().dragLine();
		editor.getDisplayList().redrawDragLine();

		editor.getDisplayList().setDragLine(dragStart.x, dragStart.y,
						    dst.getInletAnchorX(inlet), dst.getInletAnchorY(inlet));
		editor.getDisplayList().redrawDragLine();
	      }
	    else 
	      {
		destinationChoosen = false;
		editor.resetHighlightedInlet();
		dst = null;

		editor.getDisplayList().dragLine();
		editor.getDisplayList().redrawDragLine();
		editor.getDisplayList().setDragLine(dragStart.x, dragStart.y, mouse.x, mouse.y);
		editor.getDisplayList().redrawDragLine();
	      }
	  }
      }
    else if (Squeack.isDrag(squeack))
      {
	dragged = true;
	if (destinationChoosen)
	  {
	    editor.resetHighlightedInlet();
	    destinationChoosen = false;
	  }

	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(dragStart.x, dragStart.y, mouse.x, mouse.y);
	editor.getDisplayList().redrawDragLine();
      }
  }
}





