package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling Moves cursor changes; 
  Note that each interaction should set its initial cursor.
  */


class FollowInteraction extends Interaction
{
  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true);
  }

  void gotSqueack(ErmesSketchPad editor, int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    if (Squeack.isMove(squeack))
      {
	if (Squeack.onBackground(squeack))
	  editor.setCursor(Cursor.getDefaultCursor());
	if (Squeack.onObject(squeack))
	  editor.setCursor(Cursor.getDefaultCursor());
	if (Squeack.onConnection(squeack))
	  editor.setCursor(Cursor.getDefaultCursor());
	if (Squeack.onHResizeHandle(squeack))
	  editor.setCursor( Cursor.getPredefinedCursor( Cursor.E_RESIZE_CURSOR));
	if (Squeack.onVResizeHandle(squeack))
	  editor.setCursor( Cursor.getPredefinedCursor( Cursor.S_RESIZE_CURSOR));
	if (Squeack.onInlet(squeack))
	  editor.setCursor(Cursor.getDefaultCursor());
	if (Squeack.onOutlet(squeack))
	  editor.setCursor( Cursor.getPredefinedCursor( Cursor.CROSSHAIR_CURSOR));
	if (Squeack.onText(squeack))
	  editor.setCursor(Cursor.getDefaultCursor());
      }

    editor.endInteraction();
  }
}




