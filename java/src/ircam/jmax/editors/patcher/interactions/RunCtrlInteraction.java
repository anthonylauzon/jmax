package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling moving controllers in Run mode; 
  started by a control click on an object;
  Note that since the proper event sequence is guaranteed by the
  input system (i.e. Ctrl-Down [Drag]* Up), we don't need to keep track
  of the status here, so we have no real state-machine.
  */


class RunCtrlInteraction extends SubInteraction
{
  ErmesObject object;

  RunCtrlInteraction(InteractionEngine engine, Interaction master)
  {
    super(engine, master);
  }

  void configureInputFilter(InputFilter filter)
  {
    filter.setFollowingMoves(true);
  }

  void gotSqueack(int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    // Take away the control modifier (always there)

    squeack &= (~ Squeack.LOCATION_MASK);
    
    switch (squeack)
      {
      case Squeack.DOWN:
	this.object = (ErmesObject) dobject;
	object.gotSqueack(squeack, mouse, oldMouse);
	break;

      case Squeack.UP:
	object.gotSqueack(squeack, mouse, oldMouse);
	end();
	break;

      default:
	object.gotSqueack(squeack, mouse, oldMouse);
      }
  }
}



