package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling moving objects in edit mode; 
  As a special case, can open the editfield on the object
  content if the move didn't worked out.
  */


class MoveEditInteraction extends SubInteraction
{
  ErmesObject object;
  boolean dragged;

  MoveEditInteraction(InteractionEngine engine, Interaction master)
  {
    super(engine, master);
  }

  void configureInputFilter(InputFilter filter)
  {
    filter.setFollowingMoves(true);
    filter.setAutoScrolling(true);
  }

  void gotSqueack(int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    switch (squeack)
      {
      case (Squeack.DOWN | Squeack.TEXT):
	
	object = ((SensibilityArea) dobject).getObject();
	engine.getDisplayList().objectToFront(object);
	dragged = false;

	break;

      case Squeack.DRAG:

	if (! dragged)
	  {
	    engine.getSketch().setCursor(Cursor.getDefaultCursor());

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
	engine.getSketch().fixSize(); 
	break;

      case Squeack.UP:
	if (! dragged)
	  {
	    if (! ErmesSelection.patcherSelection.isEmpty() )
	      {
		ErmesSelection.patcherSelection.redraw();
		ErmesSelection.patcherSelection.deselectAll();
	      }

	    engine.getSketch().textEditObject((ErmesObjEditableObject)object, mouse);
	  }
	end();
	break;
      }
  }
}



