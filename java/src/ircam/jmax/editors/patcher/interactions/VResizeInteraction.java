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


class VResizeInteraction extends SubInteraction
{
  ErmesObject object;

  VResizeInteraction(InteractionEngine engine, Interaction master)
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


  void gotSqueack(int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    VResizeSensibilityArea area = (VResizeSensibilityArea) dobject;

    switch (squeack)
      {
      case (Squeack.DOWN | Squeack.VRESIZE_HANDLE):
	object = area.getObject();
	engine.getSketch().setCursor( Cursor.getPredefinedCursor( Cursor.S_RESIZE_CURSOR));
	break;

      case Squeack.DRAG:
	object.redraw();
	object.resizeBy(0, mouse.y - oldMouse.y);
	object.redraw();
	break;

      case Squeack.UP:
	engine.getSketch().setCursor(Cursor.getDefaultCursor());
	end();
      }
  }
}



