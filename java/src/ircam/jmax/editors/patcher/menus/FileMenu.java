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

package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.actions.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;
import ircam.jmax.utils.*;

/** Implement the patcher editor File Menu */

public class FileMenu extends DefaultFileMenu
{
  public FileMenu(ErmesSketchPad sketch)
  {
    if(sketch.isARootPatcher())
      {
	insert(Actions.saveAction, "Save", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_S, 3);
	insert(Actions.saveAsAction, "SaveAs", 4);
      }
    else
      if(sketch.isASubPatcher())
	{
	    insert(Actions.saveAction, "Save", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_S, 3);
	  insert(Actions.saveToAction, "Save As Template", 4);
	}
      else //is a template
	{
	  insert(Actions.saveTemplateAction, "Save Template", 3);
	  insert(Actions.saveToAction, "SaveAs", 4);
	}
     insertSeparator(6);
     insert(Actions.printAction, "Print", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_P, 7);
  }
}

