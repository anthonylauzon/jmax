package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;


/** 
  The Master interaction for the run mode.
  Mostly, each method install a slave interaction,
  preallocated in the constructor, and dispatch the event
  to it.
  */


class IdleRunInteraction extends Interaction
{
  // Standard interaction in run mode.
  // To add a new one, just

  Interaction helpInteraction;
  Interaction runCtrlInteraction;
  Interaction doubleClickEditInteraction;

  IdleRunInteraction(InteractionEngine engine)
  {
    super(engine);

    helpInteraction    = new HelpInteraction(engine, this);
    runCtrlInteraction = new RunCtrlInteraction(engine, this);
    doubleClickEditInteraction = new DoubleClickEditInteraction(engine, this);
  }

  void configureInputFilter(InputFilter filter)
  {
    filter.setFollowingLocations(true);
  }

  void gotSqueack(int squeack, DisplayObject object, Point mouse, Point oldMouse)
  {
    switch (squeack)
      {
      case (Squeack.ALT | Squeack.DOWN | Squeack.OBJECT):
	// Help
	engine.setInteraction(helpInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      case (Squeack.DOWN | Squeack.OBJECT):
	// Normal controller operations
	engine.setInteraction(runCtrlInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      case (Squeack.DOUBLE_CLICK | Squeack.OBJECT):
	// Edit Content 
	engine.setInteraction(doubleClickEditInteraction);
	engine.getInteraction().gotSqueack(squeack, object, mouse, oldMouse);
	break;
      }
  }
}
