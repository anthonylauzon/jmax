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


class AddObjectInteraction extends Interaction
{
  AddObjectInteraction(InteractionEngine engine)
  {
    super(engine);
  }

  void configureInputFilter(InputFilter filter)
  {
    filter.setFollowingMoves(true);
  }

  void gotSqueack(int squeack, DisplayObject object, Point mouse, Point oldMouse)
  {
    switch (squeack)
      {
      case Squeack.DOWN:
	// Help
	engine.getSketch().getToolBar().makeObject(mouse.x, mouse.y);
	engine.setEditMode();
	break;
      }
  }
}
