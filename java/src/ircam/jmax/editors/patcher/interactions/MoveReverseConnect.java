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

package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** Make a connection from an inlet to an outlet */

class MoveReverseConnect extends Interaction
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
    filter.setFollowingOutletLocations(true);
    filter.setFollowingAlsoConnectionLocations(false);
    filter.setAutoScrolling(true);
    filter.setKeyListening(true);  
  }

  // Utility method

  private void doConnection(ErmesSketchPad editor, GraphicObject src, int outlet, GraphicObject dst, int inlet)
  {
    if (src == dst)
      {
	editor.showMessage("Cannot connect an object to itself !");
	return;
      }
    
    editor.addingConnection();
    editor.getFtsPatcher().requestAddConnection(src.getFtsObject(), outlet, dst.getFtsObject(), inlet);    
  }

  public void setDest(GraphicObject obj, int in){
    dst = obj;
    inlet = in;
    moveStart.x = dst.getInletAnchorX(inlet);
    moveStart.y = dst.getInletAnchorY(inlet);
  }

  boolean destinationChoosen = false;

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isUp(squeack) && Squeack.onInlet(squeack))
      {
	ErmesSelection.patcherSelection.redraw();
	ErmesSelection.patcherSelection.deselectAll();

	dst   = (GraphicObject) area.getTarget();
	inlet = area.getNumber();

	moveStart.x = dst.getInletAnchorX(inlet);
	moveStart.y = dst.getInletAnchorY(inlet);
	editor.setConnectingObject(dst);
	//editor.resetHighlightedInlet();
      }
    else if (Squeack.isDown(squeack) && Squeack.isShift(squeack))
      {
	//  do the connection if we have a destination

	if (destinationChoosen)
	  {
	    editor.resetHighlightedOutlet(); 
	    doConnection(editor, src, outlet, dst, inlet);
	  }
      }
    else if (Squeack.isDown(squeack))
      {
	//  do the connection if we have a destination

	if (destinationChoosen)
	  {
	    editor.resetHighlightedOutlet(); 
	    editor.resetHighlightedInlet();
	    doConnection(editor, src, outlet, dst, inlet);
	  }

	// clean up
	editor.getDisplayList().noDrag();
	editor.getDisplayList().redrawDragLine();
	editor.setCursor(Cursor.getDefaultCursor());
	editor.setConnectingObject(null);
	destinationChoosen = false;
	
	editor.endInteraction();
      }
    else if (Squeack.isMove(squeack) && Squeack.onOutlet(squeack))
      {
	if ((! destinationChoosen) || src != (GraphicObject) area.getTarget() || outlet != area.getNumber())
	  {

	    if(!editor.getDisplayList().thisConnectionExist((GraphicObject) area.getTarget(), area.getNumber(),
							    dst, inlet))
	      {
		src    = (GraphicObject) area.getTarget();
		outlet = area.getNumber();
		editor.setHighlightedOutlet(src, outlet);
		destinationChoosen = true;
		editor.getDisplayList().dragLine();
		editor.getDisplayList().redrawDragLine();
		editor.getDisplayList().setDragLine(moveStart.x, moveStart.y, 
						    src.getOutletAnchorX(outlet), src.getOutletAnchorY(outlet));
		editor.getDisplayList().redrawDragLine();
	      }
	    else 
	      {
		destinationChoosen = false;
		editor.resetHighlightedOutlet();
		src = null;
 
		editor.getDisplayList().dragLine();
		editor.getDisplayList().redrawDragLine();
		editor.getDisplayList().setDragLine(moveStart.x, moveStart.y, mouse.x, mouse.y);
		editor.getDisplayList().redrawDragLine();
	      }
	  }
      }
    else if (Squeack.isMove(squeack))
      {
	if (destinationChoosen)
	  {
	    editor.resetHighlightedOutlet();
	    destinationChoosen = false;
	  }

	editor.getDisplayList().dragLine();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().setDragLine(moveStart.x, moveStart.y, mouse.x, mouse.y);
	editor.getDisplayList().redrawDragLine();
      }
    else if((squeack==Squeack.SHIFT_UP)||(squeack==Squeack.ESCAPE)||(squeack==Squeack.DELETE))
      {
	editor.resetHighlightedInlet();
	editor.getDisplayList().redrawDragLine();
	editor.getDisplayList().noDrag();
	editor.setConnectingObject(null);
	destinationChoosen = false;
	
	editor.endInteraction();
      }
  }
}



