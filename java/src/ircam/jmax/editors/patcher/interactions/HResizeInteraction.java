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


class HResizeInteraction extends Interaction
{
  GraphicObject object;

  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true);
    filter.setAutoScrolling(true);
  }
  int dx;
  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack) && Squeack.onHResizeHandle(squeack))
      {
	object = (GraphicObject) area.getTarget();
	editor.setCursor( Cursor.getPredefinedCursor( Cursor.E_RESIZE_CURSOR));

	if((!object.isSelected())||
	   ((object.isSelected())&&(ErmesSelection.patcherSelection.isSingleton())))
	  {  
	    object.resizing(true);
	    editor.setUndo( "Resize", object, false, false);
	  }
	else
	  {
	    if(ErmesSelection.patcherSelection.ownedBy(editor))
	      ErmesSelection.patcherSelection.apply(new ObjectAction() {
		  public void processObject(GraphicObject obj)
		  {
		    obj.resizing(true);
		  }});
	    editor.setUndo( "Resize", false, false);
	  }
      }
    else if (Squeack.isDrag(squeack))
      {	  
	  if(!Squeack.isShift(squeack))
	      {
		  if((!object.isSelected())||
		     ((object.isSelected())&&(ErmesSelection.patcherSelection.isSingleton()))){
		      object.redraw();
		      object.redrawConnections();
		      object.setWidth(mouse.x - object.getX());
		      object.redraw();
		      object.redrawConnections();
		  }
		  else{
		      if (ErmesSelection.patcherSelection.ownedBy(editor))
			  {
			      dx = mouse.x - oldMouse.x;
			      ErmesSelection.patcherSelection.apply(new ObjectAction() {
				      public void processObject(GraphicObject obj)
				      {
					  obj.redraw();
					  obj.redrawConnections();
					  obj.setWidth(obj.getWidth() + dx);
					  obj.redraw();
					  obj.redrawConnections();
				      }});
			  }
		  }
	      }
	  else
	      {
		  if((!object.isSelected())||
		     ((object.isSelected())&&(ErmesSelection.patcherSelection.isSingleton()))){
		      object.redraw();
		      object.redrawConnections();
		      object.setWidthShift(mouse.x - object.getX());
		      object.redraw();
		      object.redrawConnections();
		  }
		  else{
		      if (ErmesSelection.patcherSelection.ownedBy(editor))
			  {
			      dx = mouse.x - oldMouse.x;
			      ErmesSelection.patcherSelection.apply(new ObjectAction() {
				      public void processObject(GraphicObject obj)
				      {
					  obj.redraw();
					  obj.redrawConnections();
					  obj.setWidthShift(obj.getWidth() + dx);
					  obj.redraw();
					  obj.redrawConnections();
				      }});
			  }
		  }
	      }
	  editor.fixSize();
      }
    else if (Squeack.isUp(squeack))
      {
	  if((!object.isSelected())||
	     ((object.isSelected())&&(ErmesSelection.patcherSelection.isSingleton())))
	      object.resizing(false);
	  else
	      if(ErmesSelection.patcherSelection.ownedBy(editor))
		  ErmesSelection.patcherSelection.apply(new ObjectAction() {
			  public void processObject(GraphicObject obj)
			  {
			      obj.resizing(false);
			  }});
							   
	  editor.setCursor(Cursor.getDefaultCursor());
	  editor.endInteraction();
      }
  }
}



