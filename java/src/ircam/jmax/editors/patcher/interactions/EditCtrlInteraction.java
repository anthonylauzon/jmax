//
// jMax
// Copyright (C) 1999 by IRCAM
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling moving controllers in edit mode; 
  started by a control click on an object;
  Note that since the proper event sequence is guaranteed by the
  input system (i.e. Ctrl-Down [Drag]* Up), we don't need to keep track
  of the status here, so we have no real state-machine.
  */


class EditCtrlInteraction extends Interaction
{
  GraphicObject object;

  void configureInputFilter(InteractionEngine filter)
  {
    // Note that following locations is false because
    // the first event is not handled by this filter,
    // but by the master interaction filter.

    filter.setFollowingMoves(true);
  }

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    // Take away the control modifier (always there)

    squeack &= (~ Squeack.CTRL);
    squeack &= (~ Squeack.LOCATION_MASK);
    
    switch (squeack)
      {
      case Squeack.DOWN:
	object = (GraphicObject) area.getTarget();
	object.gotSqueack(squeack, mouse, oldMouse);
	break;

      case Squeack.UP:
	object.gotSqueack(squeack, mouse, oldMouse);
	editor.endInteraction();
	break;

      default:
	object.gotSqueack(squeack, mouse, oldMouse);
      }
  }
}



