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

import java.awt.*;
import java.awt.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

public class FontBiggerAction extends EditorAction
{
  public FontBiggerAction()
  {
    super("font bigger", "font bigger", KeyEvent.VK_ADD, KeyEvent.VK_ADD, true); 
  }

  public void doAction(EditorContainer container)
  {
    ErmesSketchPad sketch = (ErmesSketchPad) container.getEditor();
  
    if (ErmesSelection.patcherSelection.ownedBy(sketch)){

      sketch.setUndo("Key-Font-Change", false, true);

      ErmesSelection.patcherSelection.apply(new ObjectAction() {
	public void processObject(GraphicObject object)
	  {
	    object.redraw();
	    object.redrawConnections();
	    object.fontBigger();
	    object.redraw();
	    object.redrawConnections();
	  }});
    }
  }
}
