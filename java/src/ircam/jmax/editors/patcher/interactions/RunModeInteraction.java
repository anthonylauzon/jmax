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


/** 
  The Master interaction for the run mode.
  Mostly, each method install a slave interaction,
  preallocated in the constructor, and dispatch the event
  to it.
  */


class RunModeInteraction extends Interaction
{
  // Standard interaction in run mode.
  // To add a new one, just

  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingLocations(true);
  }

  Interaction delegateSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    switch (squeack)
      {
      case (Squeack.ALT | Squeack.DOWN | Squeack.TEXT):
      case (Squeack.ALT | Squeack.DOWN | Squeack.OBJECT):
	// Help
	return Interactions.helpInteraction;
      case (Squeack.DOWN | Squeack.OBJECT):
	// Normal controller operations
	return Interactions.runCtrlInteraction;
      case (Squeack.DOWN | Squeack.TEXT):
	// Normal controller operations
	return Interactions.runCtrlInteraction;
      case (Squeack.DOUBLE_CLICK | Squeack.OBJECT):
	// Edit Content
	return Interactions.doubleClickEditInteraction;
      case (Squeack.DOUBLE_CLICK | Squeack.TEXT):
	// Edit Content
	return Interactions.doubleClickEditInteraction;
      default :
	return null;
      }
  }
}
