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

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling drag-select; 
  started by a click on the background;
  Note that since the proper event sequence is guaranteed by the
  input system (i.e. Down [Drag]* Up), we don't need to keep track
  of the status here, so we have no real state-machine.
  */


class VResizeInteraction extends Interaction
{
  GraphicObject object;

  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true);
    filter.setAutoScrolling(true);
  }

  int dy;
  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack) && Squeack.onVResizeHandle(squeack))
      {
	object = (GraphicObject) area.getTarget();
	editor.setCursor( Cursor.getPredefinedCursor( Cursor.S_RESIZE_CURSOR));
      }
    else if (Squeack.isDrag(squeack))
      {
	object.redraw();
	object.redrawConnections();
	object.setHeight(mouse.y - object.getY());
	object.redraw();
	object.redrawConnections();

	if (ErmesSelection.patcherSelection.ownedBy(editor))
	  {
	    dy = mouse.y - oldMouse.y;
	    ErmesSelection.patcherSelection.apply(new ObjectAction() {
	      public void processObject(GraphicObject obj)
		{
		  if(obj!=object){
		    obj.redraw();
		    obj.redrawConnections();
		    obj.setHeight(object.getHeight() + dy);
		    obj.redraw();
		    obj.redrawConnections();
		  }
		}});
	  }

	editor.fixSize(); 
      }
    else if (Squeack.isUp(squeack))
      {
	editor.setCursor(Cursor.getDefaultCursor());
	editor.endInteraction();
      }
  }
}



