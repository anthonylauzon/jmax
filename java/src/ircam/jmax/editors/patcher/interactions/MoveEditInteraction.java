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
  boolean editAtUp;

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
      case (Squeack.DOWN | Squeack.OBJECT):
	
	object = (ErmesObject) dobject;
	engine.getDisplayList().objectToFront(object);

	if (object.isSelected())
	  {
	    editAtUp = true;
	  }
	else
	  {
	    if (! ErmesSelection.patcherSelection.isEmpty() )
	      {
		ErmesSelection.patcherSelection.redraw();
		ErmesSelection.patcherSelection.deselectAll();
	      }

	    ErmesSelection.patcherSelection.select(object);
	    object.redraw();
	    editAtUp = false;
	  }
	engine.getSketch().setCursor(Cursor.getDefaultCursor());
	break;

      case Squeack.DRAG:
	ErmesSelection.patcherSelection.moveAllBy(mouse.x - oldMouse.x, mouse.y - oldMouse.y);
	engine.getSketch().fixSize(); 
	editAtUp = false;
	break;

      case Squeack.UP:
	if (editAtUp && (object instanceof ErmesObjEditableObject))
	  {
	    ErmesSelection.patcherSelection.redraw();
	    ErmesSelection.patcherSelection.deselectAll();
	    engine.getSketch().textEditObject((ErmesObjEditableObject)object, mouse);
	  }
	end();
	break;

      default:
	object.gotSqueack(squeack, mouse, oldMouse);
      }
  }
}



