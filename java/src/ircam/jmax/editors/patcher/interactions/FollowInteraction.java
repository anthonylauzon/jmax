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

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;

/** The interaction handling Moves cursor changes; 
  Note that each interaction should set its initial cursor.
  */


class FollowInteraction extends Interaction
{
  void configureInputFilter(InteractionEngine filter)
  {
    filter.setFollowingMoves(true);
  }

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (Squeack.isMove(squeack))
      {
	editor.resetHighlightedInlet();
	editor.resetHighlightedOutlet();

	if (Squeack.onBackground(squeack))
	  {
	    editor.resetMessage();
	    editor.setCursor(Cursor.getDefaultCursor());
	  }
	else if (Squeack.onObject(squeack))
	  {
	    editor.setCursor(Cursor.getDefaultCursor());
	    ((GraphicObject) area.getTarget()).assistOnObject();
	  }
	else if (Squeack.onConnection(squeack))
	  {
	    editor.resetMessage();
	    editor.setCursor(Cursor.getDefaultCursor());
	  }
	else if (Squeack.onHResizeHandle(squeack))
	  {
	    editor.resetMessage();
	    editor.setCursor( Cursor.getPredefinedCursor( Cursor.E_RESIZE_CURSOR));
	  }
	else if (Squeack.onVResizeHandle(squeack))
	  {
	    editor.resetMessage();
	    editor.setCursor( Cursor.getPredefinedCursor( Cursor.S_RESIZE_CURSOR));
	  }
	else if (Squeack.onInlet(squeack))
	  {
	    if (! editor.isHighlightedInlet((GraphicObject) area.getTarget(), area.getNumber()))
	      {
		editor.setHighlightedInlet((GraphicObject) area.getTarget(), area.getNumber());
		editor.setCursor(Cursor.getDefaultCursor());
	      }

	    ((GraphicObject) area.getTarget()).assistOnInlet(area.getNumber());
	  }
	else if (Squeack.onOutlet(squeack))
	  {
	    if (! editor.isHighlightedOutlet((GraphicObject) area.getTarget(), area.getNumber()))
	      {
		editor.setHighlightedOutlet((GraphicObject) area.getTarget(), area.getNumber());
		editor.setCursor(Cursor.getDefaultCursor());
	      }

	    ((GraphicObject) area.getTarget()).assistOnOutlet(area.getNumber());
	  }
	else if (Squeack.onText(squeack))
	  {
	    ((GraphicObject) area.getTarget()).assistOnObject();
	    editor.setCursor(Cursor.getDefaultCursor());
	  }
      }

    editor.endInteraction();
  }
}




