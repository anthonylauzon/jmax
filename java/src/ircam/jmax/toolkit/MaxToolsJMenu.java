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

package ircam.jmax.toolkit;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;

public class MaxToolsJMenu extends JMenu implements ListDataListener 
{
  ListModel toolFinders;

  class ToolActionListener implements ActionListener
  {
    JMenuItem item;
    MaxToolFinder toolFinder;

    ToolActionListener(JMenuItem item,  MaxToolFinder toolFinder)
    {
      this.item = item;
      this.toolFinder = toolFinder;
    }

    public  void actionPerformed(ActionEvent e)
    {
      toolFinder.open();
    }
  }

  /** Build a tool menu for frame */

  public MaxToolsJMenu(String title)
  {
    super(title);
    setHorizontalTextPosition(AbstractButton.LEFT);
    
    toolFinders = MaxWindowManager.getWindowManager().getToolFinderList();
    toolFinders.addListDataListener(this);

    rebuildToolsMenu();
  }

  private void rebuildToolsMenu()
  {
    JMenuItem mi;

    if (getMenuComponentCount() > 0)
      removeAll();

    // Built the tool menu

    for (int i = 0; i < toolFinders.getSize(); i++)
      {
	MaxToolFinder toolFinder;

	toolFinder = (MaxToolFinder) toolFinders.getElementAt(i);

	mi = new JMenuItem(toolFinder.getToolName());
	add(mi);
	mi.addActionListener(new ToolActionListener(mi, toolFinder));
      }
  }

  // ListDataListener

  public void contentsChanged(ListDataEvent e)
  {
    rebuildToolsMenu();
  }

  public void intervalAdded(ListDataEvent e)
  {
    rebuildToolsMenu();
  }

  public void intervalRemoved(ListDataEvent e) 
  {
    rebuildToolsMenu();
  }
}




                  

