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
  ErmesObject object;

  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true);
    filter.setAutoScrolling(true);
  }

  void gotSqueack(ErmesSketchPad editor, int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack) && Squeack.onObject(squeack))
      {
	object = (ErmesObject) dobject;
	editor.getDisplayList().objectToFront(object);

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



