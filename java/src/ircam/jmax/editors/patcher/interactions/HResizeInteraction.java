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


class HResizeInteraction extends SubInteraction
{
  ErmesObject object;

  HResizeInteraction(InteractionEngine engine, Interaction master)
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
    HResizeSensibilityArea area = (HResizeSensibilityArea) dobject;

    switch (squeack)
      {
      case (Squeack.DOWN | Squeack.HRESIZE_HANDLE):
	object = area.getObject();
	engine.getSketch().setCursor( Cursor.getPredefinedCursor( Cursor.E_RESIZE_CURSOR));
	break;

      case Squeack.DRAG:
	object.redraw();
	object.setWidth(mouse.x - object.getX());
	object.redraw();
	engine.getSketch().fixSize(); 
	break;

      case Squeack.UP:
	engine.getSketch().setCursor(Cursor.getDefaultCursor());
	end();
      }
  }
}



