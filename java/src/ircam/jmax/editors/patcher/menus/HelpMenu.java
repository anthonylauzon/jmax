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

package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;

import ircam.jmax.editors.patcher.*;

/** Implement the patcher editor File Menu */

public class HelpMenu extends JMenu
{
  ErmesSketchWindow editor;

  public HelpMenu(ErmesSketchWindow window)
  {
    super("Help");
    setHorizontalTextPosition(AbstractButton.LEFT);

    JMenuItem item;
    editor = window;

    item = new JMenuItem( "Help Patch", KeyEvent.VK_H);
    add( item);
    item.addActionListener( new ActionListener() {
      public  void actionPerformed( ActionEvent e)
	{
	  if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
	    {
	      if (! ErmesSelection.patcherSelection.openHelpPatches())
		new ErrorDialog( editor, "Sorry, no help for object ");
	    }
	}
    });

    // Adding the summaries 

    Enumeration en = FtsHelpPatchTable.getSummaries(); 

    while (en.hasMoreElements())
       {
	 final String str = (String) en.nextElement();

	item = new JMenuItem( str + " summary");
	add( item);
	item.addActionListener( new ActionListener() {
	  public  void actionPerformed( ActionEvent e)
	    {
	      FtsHelpPatchTable.openHelpSummary(editor.getFts(), str);
	    }
	});
       }
  
}
}
