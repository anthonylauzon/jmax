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

package ircam.jmax.editors.patcher.actions;

import java.util.*;
import java.awt.event.KeyEvent;

import ircam.fts.client.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

public class OpenHelpPatchAction extends EditorAction
{
   public OpenHelpPatchAction()
	  {
		 super("Object help...", null, KeyEvent.VK_O, KeyEvent.VK_F1, true);
	  }
  public void doAction(EditorContainer container)
  {
    FtsObject object;
    if (ErmesSelection.patcherSelection.ownedBy( (ErmesSketchPad)container.getEditor()))
      {      
	FtsPatcherObject patcher = ((ErmesSketchPad)container.getEditor()).getFtsPatcher();
	for (Enumeration en = ErmesSelection.patcherSelection.getSelectedObjects(); en.hasMoreElements(); )
	  {
	    object = ((GraphicObject) en.nextElement()).getFtsObject();	
	    patcher.requestOpenHelpPatch( object);
	  }
      }
  }
}


