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

  void gotSqueack(ErmesSketchPad editor, int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    switch (squeack)
      {
      case Squeack.DOWN:
	// Help
	editor.getToolBar().makeObject(mouse.x, mouse.y);
	editor.setEditModeInteraction();
	break;
      }
  }
}
