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
  ErmesObject object;

  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true);
    filter.setAutoScrolling(true);
  }

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack) && Squeack.onHResizeHandle(squeack))
      {
	object = (ErmesObject) area.getTarget();
	editor.setCursor( Cursor.getPredefinedCursor( Cursor.E_RESIZE_CURSOR));
      }
    else if (Squeack.isDrag(squeack))
      {
	object.redraw();
	object.setWidth(mouse.x - object.getX());
	object.redraw();
	editor.fixSize();
      }
    else if (Squeack.isUp(squeack))
      {
	editor.setCursor(Cursor.getDefaultCursor());
	editor.endInteraction();
      }
  }
}



