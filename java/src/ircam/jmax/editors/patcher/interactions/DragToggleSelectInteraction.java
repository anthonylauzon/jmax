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


class DragToggleSelectInteraction extends Interaction
{
  Point dragStart = new Point();
  boolean dragged = false;
  boolean bringToBack = false;
  ErmesObject object;

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
	editor.getDisplayList().toggleSelect(editor.getDisplayList().getDragRectangle());
	object = (ErmesObject) area.getTarget();
	bringToBack = true;
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
	  }

	editor.endInteraction();
      }
  }
}







