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

package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling moving objects in edit mode; 
  As a special case, can open the editfield on the object
  content if the move didn't worked out.
  */


class MoveInteraction extends Interaction
{
  GraphicObject object;

  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true);
    filter.setAutoScrolling(true);
  }

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack) && Squeack.onObject(squeack))
      {	
	object = (GraphicObject) area.getTarget();

	if (! object.isSelected())
	  {
	    if (! ErmesSelection.patcherSelection.isEmpty() )
	      {
		ErmesSelection.patcherSelection.redraw();
		ErmesSelection.patcherSelection.deselectAll();
	      }
	    ErmesSelection.patcherSelection.select(object);
	    object.redraw();	    
	  }

	editor.setUndo( "Move", false, false);

	editor.setCursor(Cursor.getDefaultCursor());
      }
    else if (Squeack.isDrag(squeack))
      {
	ErmesSelection.patcherSelection.moveAllBy(mouse.x - oldMouse.x, mouse.y - oldMouse.y);
	editor.fixSize(); 
      }
    else if (Squeack.isUp(squeack))
      {
	editor.endInteraction();
      }
  }
}



