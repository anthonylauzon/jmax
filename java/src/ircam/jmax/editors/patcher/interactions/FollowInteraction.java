package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;

/** The interaction handling Moves cursor changes; 
  Note that each interaction should set its initial cursor.
  */


class FollowInteraction extends Interaction
{
  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true);
  }

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isMove(squeack))
      {
	editor.resetHighlightedInlet();
	editor.resetHighlightedOutlet();

	if (Squeack.onBackground(squeack))
	  editor.setCursor(Cursor.getDefaultCursor());
	else if (Squeack.onObject(squeack))
	  editor.setCursor(Cursor.getDefaultCursor());
	else if (Squeack.onConnection(squeack))
	  editor.setCursor(Cursor.getDefaultCursor());
	else if (Squeack.onHResizeHandle(squeack))
	  editor.setCursor( Cursor.getPredefinedCursor( Cursor.E_RESIZE_CURSOR));
	else if (Squeack.onVResizeHandle(squeack))
	  editor.setCursor( Cursor.getPredefinedCursor( Cursor.S_RESIZE_CURSOR));
	else if (Squeack.onInlet(squeack))
	  {
	    if (! editor.isHighlightedInlet((ErmesObject) area.getTarget(), area.getNumber()))
	      {
		editor.setHighlightedInlet((ErmesObject) area.getTarget(), area.getNumber());
		editor.setCursor(Cursor.getDefaultCursor());
	      }
	  }
	else if (Squeack.onOutlet(squeack))
	  {
	    if (! editor.isHighlightedOutlet((ErmesObject) area.getTarget(), area.getNumber()))
	      {
		editor.setHighlightedOutlet((ErmesObject) area.getTarget(), area.getNumber());
		editor.setCursor(Cursor.getDefaultCursor());
	      }
	  }
	else if (Squeack.onText(squeack))
	  editor.setCursor(Cursor.getDefaultCursor());
      }

    editor.endInteraction();
  }
}




