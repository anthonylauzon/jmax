package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.editors.patcher.*;

/** The interaction handling drag-select; 
  started by a click on the background;
  Note that since the proper event sequence is guaranteed by the
  input system (i.e. Down [Drag]* Up), we don't need to keep track
  of the status here, so we have no real state-machine.
  */


class DragToggleSelectInteraction extends SubInteraction
{
  Point dragStart = new Point();
  boolean dragged = false;

  DragToggleSelectInteraction(InteractionEngine engine, Interaction master)
  {
    super(engine, master);
  }

  void configureInputFilter(InputFilter filter)
  {
    // Note that following locations is false because
    // the first event is not handled by this filter,
    // but by the master interaction filter.

    filter.setFollowingMoves(true); // need the drag
    filter.setFollowingLocations(false);
  }

  void gotSqueack(int squeack, DisplayObject object, Point mouse, Point oldMouse)
  {
    switch (squeack)
      {
      case (Squeack.SHIFT | Squeack.DOWN | Squeack.OBJECT):
	// Fake a drag of 1 pixel, to select the object we are in
	// But do not draw the rectangle

	dragged = true;
	dragStart.setLocation(mouse);
	engine.getDisplayList().setDragRectangle(dragStart.x, dragStart.y, 1, 1);
	engine.getDisplayList().toggleSelect(engine.getDisplayList().getDragRectangle());
	break;

      case (Squeack.SHIFT | Squeack.DOWN | Squeack.BACKGROUND):

	dragged = false;
	dragStart.setLocation(mouse);
	break;

      case (Squeack.SHIFT | Squeack.DRAG):
      case Squeack.DRAG:
	if (dragged)
	  {
	    engine.getDisplayList().toggleSelect(engine.getDisplayList().getDragRectangle());
	    engine.getDisplayList().redrawDragRectangle();
	  }

	engine.getDisplayList().setDragRectangle(dragStart.x, dragStart.y,
						 mouse.x - dragStart.x, mouse.y - dragStart.y);
	engine.getDisplayList().dragRectangle();
	engine.getDisplayList().toggleSelect(engine.getDisplayList().getDragRectangle());
	engine.getDisplayList().redrawDragRectangle();
	dragged = true;
	break;

      case  Squeack.UP:
      case (Squeack.SHIFT | Squeack.UP):
	if (dragged)
	  {
	    engine.getDisplayList().noDrag();
	    engine.getDisplayList().redrawDragRectangle();
	  }

	end();
	break;
      }
  }
}







