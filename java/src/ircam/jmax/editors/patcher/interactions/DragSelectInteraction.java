package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.editors.patcher.*;

/** The interaction handling drag-select; 
  started by a click on the background;
  Note that since the proper event sequence is guaranteed by the
  input system (i.e. Down [Drag]* Up), we don't need to keep track
  of the status here, so we have no real state-machine.
  */


class DragSelectInteraction extends Interaction
{
  Point dragStart = new Point();
  boolean dragged = false;

  void configureInputFilter(InteractionEngine filter)
  {
    // Note that following locations is false because
    // the first event is not handled by this filter,
    // but by the master interaction filter.

    filter.setFollowingMoves(true);
    filter.setAutoScrolling(true);
  }

  void gotSqueack(ErmesSketchPad editor, int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    switch (squeack)
      {
      case (Squeack.DOWN | Squeack.BACKGROUND):
	dragged = false;
	dragStart.setLocation(mouse);
	break;

      case Squeack.DRAG:
	if (dragged)
	  editor.getDisplayList().redrawDragRectangle();

	editor.getDisplayList().setDragRectangle(dragStart.x, dragStart.y,
						 mouse.x - dragStart.x, mouse.y - dragStart.y);
	editor.getDisplayList().dragRectangle();
	editor.getDisplayList().selectExactly(editor.getDisplayList().getDragRectangle());
	editor.getDisplayList().redrawDragRectangle();
	dragged = true;
	break;

      case Squeack.UP:
	if (dragged)
	  {
	    editor.getDisplayList().noDrag();
	    editor.getDisplayList().redrawDragRectangle();
	  }
	else
	  {
	    ErmesSelection.patcherSelection.redraw();
	    ErmesSelection.patcherSelection.deselectAll();
	  }

	editor.endInteraction();
	break;
      }
  }
}







