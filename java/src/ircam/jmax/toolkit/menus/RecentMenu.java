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
// Authors: Maurizio Umberto Puxeddu.
// 

package ircam.jmax.toolkit.menus;

import java.io.File;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.actions.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;
import ircam.jmax.toolkit.actions.OpenAction;

/** Implement the editor Recent File sub-menu */

public class RecentMenu extends EditorMenu implements ListDataListener
{
    public RecentMenu()
    {
	super("Recent files");
	setHorizontalTextPosition(AbstractButton.LEFT);

	RecentFileHistory recentFileHistory = MaxApplication.getRecentFileHistory();
	recentFileHistory.addListDataListener(this);

	build();
    }

    // Rebuild menu from recent file history (may be optimized)

    void build()
    {
	File file;
	JMenuItem jMenuItem;
	RecentFileHistory recentFileHistory = MaxApplication.getRecentFileHistory();
	
	removeAll();

	for (int i = 0; i < recentFileHistory.size(); ++i)
	    {
		file = (File)recentFileHistory.get(i);

		jMenuItem = new JMenuItem(file.getName());
		add(jMenuItem);

		jMenuItem.addActionListener(new OpenAction(file));
		
	    }
    }


    // ListDataListener implementation follows:

    public void contentsChanged(ListDataEvent e)
    {
	build();
    }
    
    public void intervalAdded(ListDataEvent e)
    {
	build();
    }
    
    public void intervalRemoved(ListDataEvent e) 
    {
	build();
    }
}
