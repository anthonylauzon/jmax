package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;


/** 
  The Master interaction for the add mode.
  The add mode is switched in when a button is clicked on the object palette.
  This interaction just handle the mouse down directly, and then go back
  to the edit mode.
  */


class AddModeInteraction extends Interaction
{
  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true);
  }

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    switch (squeack)
      {
      case (Squeack.DOWN | Squeack.SHIFT):
	editor.makeAddModeObject(mouse.x, mouse.y, false);
	break;

    case Squeack.DOWN:
	editor.makeAddModeObject(mouse.x, mouse.y, true);
	editor.setCursor(Cursor.getDefaultCursor());
	editor.resetMessage();
	editor.setEditModeInteraction();
	break;

    case Squeack.POP_UP:
	editor.setCursor(Cursor.getDefaultCursor());
	editor.resetMessage();
	editor.setEditModeInteraction();
	break;
      }
  }
}
