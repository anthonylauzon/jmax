//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** Make a connection from an outlet to an inlet */


class MoveConnectInteraction extends Interaction
{
  Point moveStart = new Point();
  GraphicObject src;
  int outlet;
  GraphicObject dst;
  int inlet;

  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true); // need the drag
    filter.setFollowingInOutletLocations(true);
    filter.setAutoScrolling(true);
  }

  // Utility method

  private void doConnection(ErmesSketchPad editor, GraphicObject src, int outlet, GraphicObject dst, int inlet)
  {
    try
      {
	FtsConnection fc;
	GraphicConnection connection;

	fc = editor.getFts().makeFtsConnection(src.getFtsObject(),
				   outlet, 
				   dst.getFtsObject(),
				   inlet);
		    
	connection = new GraphicConnection(editor,
					 src,
					 outlet,
					 dst,
					 inlet,
					 fc);
		
	editor.getDisplayList().add(connection);
	editor.getDisplayList().sortDisplayList();
	connection.updateDimensions();
	connection.redraw();
      }
    catch (FtsException e)
      {
	// Just don't do the connection in case of troubles.

	// editor.showMessage("Cannot connect");
      }
  }

  boolean destinationChoosen = false;

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isUp(squeack) && Squeack.onOutlet(squeack))
      {
	ErmesSelection.patcherSelection.redraw();
	ErmesSelection.patcherSelection.deselectAll();

	src    = (GraphicObject) area.getTarget();
	outlet = area.getNumber();

	moveStart.x = src.getOutletAnchorX(outlet);
	moveStart.y = src.getOutletAnchorY(outlet);
	editor.resetHighlightedOutlet();
      }
    else if (Squeack.isDown(squeack) && Squeack.isShift(squeack))
      {
	if (destinationChoosen)
	  {
	    editor.resetHighlightedInlet(); 
	    doConnection(editor, src, outlet, dst, inlet);
	  }
      }
    else if (Squeack.isDown(squeack))
      {
	if (destinationChoosen)
	  {
	    editor.resetHighlightedInlet(); 
	    doConnection(editor, src, outlet, dst, inlet);
	  }

	// clean up

	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().noDrag();
	destinationChoosen = false;

	editor.endInteraction();
      }
    else if (Squeack.isMove(squeack) && Squeack.onInlet(squeack))
      {
	if ((! destinationChoosen) || dst != (GraphicObject) area.getTarget() || inlet != area.getNumber())
	  {
	    dst   = (GraphicObject) area.getTarget();
	    inlet = area.getNumber();

	    editor.setHighlightedInlet(dst, inlet);
	    destinationChoosen = true;
	  }


	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(moveStart.x, moveStart.y,
					    dst.getInletAnchorX(inlet), dst.getInletAnchorY(inlet));
	editor.getDisplayList().redrawDragLine();
      }
    else if (Squeack.isMove(squeack))
      {
	if (destinationChoosen)
	  {
	    editor.resetHighlightedInlet();
	    destinationChoosen = false;
	  }

	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(moveStart.x, moveStart.y, mouse.x, mouse.y);
	editor.getDisplayList().redrawDragLine();
      }
  }
}



