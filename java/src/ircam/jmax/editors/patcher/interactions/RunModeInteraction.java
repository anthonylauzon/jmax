//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
			case (Squeack.MIDDLE_BUTTON | Squeack.DOWN | Squeack.TEXT):
			case (Squeack.MIDDLE_BUTTON | Squeack.DOWN | Squeack.OBJECT):
				// Help
				return Interactions.helpInteraction;
			case (Squeack.POP_UP | Squeack.OBJECT):
				return Interactions.runPopUpInteraction;
			case (Squeack.DOWN | Squeack.OBJECT):
			case (Squeack.SHIFT | Squeack.DOWN | Squeack.OBJECT):
				// Normal controller operations
				return Interactions.runCtrlInteraction;
			case (Squeack.DOWN | Squeack.TEXT):
				// Normal controller operations
				return Interactions.runCtrlInteraction;
			case (Squeack.DOUBLE_CLICK | Squeack.OBJECT):
				// Edit Content
				return Interactions.doubleClickEdit;
			case (Squeack.DOUBLE_CLICK | Squeack.TEXT):
				// Edit Content
				return Interactions.doubleClickEdit;
			case (Squeack.SHORTCUT | Squeack.DOWN | Squeack.BACKGROUND):
			case (Squeack.SHORTCUT | Squeack.DOUBLE_CLICK | Squeack.BACKGROUND):
				editor.setLocked(false);
      default :
				return null;
		}
	}
}

