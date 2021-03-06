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

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling help patches; 
  started, and completed, by a AltClick on an object.
  */


class HelpInteraction extends Interaction
{
  boolean locked = false;

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    GraphicObject object = null;

    /* Please notes that since the ErrorDialog is modal, this interaction
       will continue to receive events, and will not execute the "endInteraction"
       until the OK button is pressed; that is why we have the locked flag; also,
       it is happening in the same thread (the show method of the error dialog)
       so there is nothing to do using synchronized and so on; we just need to ignore
       events different than the first one.
       */

    if ((! locked) && Squeack.isDown(squeack))
      {
	locked = true;

	object = (GraphicObject) area.getTarget();

	if (object != null)
	  /*if (! FtsHelpPatchTable.openHelpPatch( object.getFtsObject()))
	    JOptionPane.showMessageDialog(editor.getEditorContainer().getFrame(), 
	    "Sorry, no help for object "+object.getFtsObject().getDescription(), 
	    "Warning", JOptionPane.INFORMATION_MESSAGE); */
	  editor.getFtsPatcher().requestOpenHelpPatch( object.getFtsObject());

	locked = false;
	editor.endInteraction();
      }
  }
}




