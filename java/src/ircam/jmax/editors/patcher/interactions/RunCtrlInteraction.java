//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

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


class RunCtrlInteraction extends Interaction
{
  GraphicObject object;

  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true);
  }

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isDown(squeack) && (Squeack.onObject(squeack) || Squeack.onText(squeack)))
      object = (GraphicObject) area.getTarget();

    // Take away the control modifier (always there)

    squeack &= (~ Squeack.LOCATION_MASK);


    switch (squeack)
      {
      case Squeack.DRAG:
      case Squeack.DOWN:
	object.gotSqueack(squeack, mouse, oldMouse);    
	break;

      case Squeack.UP:
	object.gotSqueack(squeack, mouse, oldMouse);
	editor.endInteraction();
	break;
      }
  }
}



