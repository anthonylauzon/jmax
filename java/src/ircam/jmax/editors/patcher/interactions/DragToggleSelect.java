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

/** The interaction handling drag-select; 
  started by a click on the background;
  Note that since the proper event sequence is guaranteed by the
  input system (i.e. Down [Drag]* Up), we don't need to keep track
  of the status here, so we have no real state-machine.
  */


class DragToggleSelect extends Interaction
{
  Point dragStart = new Point();
  boolean dragged = false;
  boolean bringToBack = false;
  GraphicObject object;

  void configureInputFilter(InteractionEngine filter)
  {
    // Note that following locations is false because
    // the first event is not handled by this filter,
    // but by the master interaction filter.

    filter.setFollowingMoves(true); 
    filter.setAutoScrolling(true);
  }

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack) && Squeack.isShift(squeack) &&
	(Squeack.onObject(squeack) || Squeack.onText(squeack)))
      {
	// Fake a drag of 1 pixel, to select the object we are in
	// But do not draw the rectangle
	
	dragged = true;
	dragStart.setLocation(mouse);
	editor.getDisplayList().setDragRectangle(dragStart.x, dragStart.y, 1, 1);
	
	object = (GraphicObject) area.getTarget();
	if ( object.isSelected())
	  ErmesSelection.patcherSelection.deselect(object);
	else
	  {
	    if( editor.isTextEditingObject())
	      {
		GraphicObject obj = editor.getTextEditedObject();
		editor.abortTextEditing();
		ErmesSelection.patcherSelection.select(obj);
	      }

	    ErmesSelection.patcherSelection.select(object);
	  }
	object.redraw();
	//to made faster the shift+click objects selection we loose "to back" feature 
	//bringToBack = true;
	bringToBack = false;
	editor.endInteraction();
      }
    else if (Squeack.isDown(squeack) && Squeack.isShift(squeack) && Squeack.onBackground(squeack))
      {
	dragged = false;
	dragStart.setLocation(mouse);
	bringToBack = false;
      }
    else if (Squeack.isDrag(squeack))
      {
	if (dragged)
	  {
	    editor.getDisplayList().toggleSelect(editor.getDisplayList().getDragRectangle());
	    editor.getDisplayList().redrawDragRectangle();
	  }

	editor.getDisplayList().setDragRectangle(dragStart.x, dragStart.y,
						 mouse.x - dragStart.x, mouse.y - dragStart.y);
	editor.getDisplayList().dragRectangle();
	editor.getDisplayList().toggleSelect(editor.getDisplayList().getDragRectangle());
	editor.getDisplayList().redrawDragRectangle();
	dragged = true;
	bringToBack = false;
      }
    else if (Squeack.isUp(squeack))
      {
	if (dragged)
	  {
	    editor.getDisplayList().noDrag();
	    editor.getDisplayList().redrawDragRectangle();
	  }

	if (bringToBack)
	  {
	    editor.getDisplayList().objectToBack(object);
	    object.redraw();
	    object.redrawConnections();
	  }

	editor.endInteraction();
      }
  }
}







