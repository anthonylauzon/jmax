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
import java.awt.event.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** 
The Master interaction for the edit mode.
Mostly, each method install a slave interaction,
preallocated in the constructor, and dispatch the event
to it.
*/


class EditModeInteraction extends Interaction
{
  void configureInputFilter(InteractionEngine filter)
{
    filter.setFollowingMoves(true);
    filter.setFollowingLocations(true);
    filter.setFollowingInOutletLocations(true);
    filter.setFollowingAlsoConnectionLocations(true);
}

Interaction delegateSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
{
	if (Squeack.isMove(squeack))
	{
		// Move
		return  Interactions.followInteraction;
	}
	else
	{
		editor.resetMessage();
		
		switch (squeack)
	  {
			case (Squeack.POP_UP | Squeack.BACKGROUND):
				// Drag Select
				editor.resetHighlighted();
				return Interactions.bgPopUpInteraction;
				
			case (Squeack.SHORTCUT | Squeack.DOWN | Squeack.BACKGROUND):
			case (Squeack.SHORTCUT | Squeack.DOUBLE_CLICK | Squeack.BACKGROUND):
				editor.setLocked(true);
				return null;
			case (Squeack.DOWN | Squeack.BACKGROUND):
				// Drag Select
				editor.resetHighlighted();
				return Interactions.dragSelectInteraction;
				
			case (Squeack.SHIFT | Squeack.DOWN | Squeack.TEXT):
			case (Squeack.SHIFT | Squeack.DOWN | Squeack.OBJECT):
			case (Squeack.SHIFT | Squeack.DOWN | Squeack.BACKGROUND):
				// Drag Select
				editor.resetHighlighted();
				return Interactions.dragToggleSelect;
				
			case (Squeack.DOWN | Squeack.OBJECT):
				// Drag Select
				editor.resetHighlighted();
				return Interactions.moveInteraction;
				
			case (Squeack.POP_UP | Squeack.OBJECT):
			case (Squeack.POP_UP | Squeack.TEXT):
			case (Squeack.POP_UP | Squeack.OUTLET):
			case (Squeack.POP_UP | Squeack.INLET):
				// Drag Select
				editor.resetHighlighted();
				return Interactions.popUpInteraction;
				
			case (Squeack.DOWN | Squeack.TEXT):
				// Drag Select
				editor.resetHighlighted();
				return Interactions.moveEditInteraction;
				
			case (Squeack.DOWN | Squeack.CONNECTION):
				// Drag Select
				editor.resetHighlighted();
				return Interactions.connSelectInteraction;
				
			case (Squeack.SHIFT | Squeack.DOWN | Squeack.CONNECTION):
				// Drag Select
				editor.resetHighlighted();
				return Interactions.connToggleSelectInteraction;
				
			case (Squeack.DOWN  | Squeack.OUTLET):
			case (Squeack.SHIFT | Squeack.DOWN | Squeack.OUTLET):
				// Drag connect
				return Interactions.dragConnectInteraction;
				
			case (Squeack.DOWN  | Squeack.INLET):
			case (Squeack.SHIFT | Squeack.DOWN | Squeack.INLET):
				// Drag connect
				return Interactions.dragReverseConnect;
				
			case (Squeack.SHORTCUT | Squeack.DOWN | Squeack.OBJECT | Squeack.SHIFT):
			case (Squeack.SHORTCUT | Squeack.DOWN | Squeack.OBJECT):
			case (Squeack.SHORTCUT | Squeack.DOWN | Squeack.TEXT):
				// Ctrl drag for controllers
				editor.resetHighlighted();
				return Interactions.editCtrlInteraction;
				
			case (Squeack.DOWN | Squeack.HRESIZE_HANDLE): 
			case (Squeack.SHIFT | Squeack.DOWN | Squeack.HRESIZE_HANDLE):
				// Horizontal Resize
				editor.resetHighlighted();
				return Interactions.hResizeInteraction;
				
			case (Squeack.DOWN | Squeack.VRESIZE_HANDLE):
				// Vertical Resize
				editor.resetHighlighted();
				return Interactions.vResizeInteraction;
				
			case (Squeack.MIDDLE_BUTTON | Squeack.DOWN | Squeack.TEXT):
			case (Squeack.MIDDLE_BUTTON | Squeack.DOWN | Squeack.OBJECT):
				// Help
				editor.resetHighlighted();
				return Interactions.helpInteraction;
				
			case (Squeack.SHORTCUT | Squeack.DOUBLE_CLICK | Squeack.TEXT):
			case (Squeack.SHORTCUT | Squeack.DOUBLE_CLICK | Squeack.OBJECT):
				// Edit Content
				editor.resetHighlighted();
				return Interactions.doubleClickEdit;
				
			default :
				return null;
	  }
	}
}
}




