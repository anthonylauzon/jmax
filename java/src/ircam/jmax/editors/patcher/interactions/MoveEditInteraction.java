package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling moving objects in edit mode; 
  As a special case, can open the editfield on the object
  content if the move didn't worked out.
  */


class MoveEditInteraction extends Interaction
{
  GraphicObject object;
  boolean dragged;

  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true);
    filter.setAutoScrolling(true);
  }

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack) && Squeack.onText(squeack))
      {
	object = (GraphicObject) area.getTarget();
	editor.getDisplayList().objectToFront(object);
	object.redraw();
	object.redrawConnections();
	dragged = false;
      }
    else if (Squeack.isDrag(squeack))
      {
	if (! dragged)
	  {
	    editor.setCursor(Cursor.getDefaultCursor());

	    dragged = true;

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
	  }

	ErmesSelection.patcherSelection.moveAllBy(mouse.x - oldMouse.x, mouse.y - oldMouse.y);
	editor.fixSize(); 
      }
    else if (Squeack.isUp(squeack))
      {
	if (! dragged)
	  {
	    if (! ErmesSelection.patcherSelection.isEmpty() )
	      {
		ErmesSelection.patcherSelection.redraw();
		ErmesSelection.patcherSelection.deselectAll();
	      }

	    object.edit(mouse);
	  }

	editor.endInteraction();
      }
  }
}



