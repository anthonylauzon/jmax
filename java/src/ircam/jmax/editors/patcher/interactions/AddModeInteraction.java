 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.patcher.interactions;

import java.awt.*;
import java.awt.event.*;

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
    filter.setKeyListening(true);  
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
	editor.resetAdding();
	break;

      case Squeack.POP_UP:
	editor.resetAdding();
	break;
      
      case Squeack.SHIFT_UP:
	editor.resetAdding();
	break;
      }
  }  
}








