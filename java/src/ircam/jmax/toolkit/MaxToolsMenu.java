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
import ircam.jmax.utils.*;

public class MaxToolsMenu extends Menu implements ListDataListener 
{
  ListModel toolFinders;
  private MaxVector toDispose = new MaxVector();

  class ToolActionListener implements ActionListener
  {
    MenuItem item;
    MaxToolFinder toolFinder;

    ToolActionListener(MenuItem item,  MaxToolFinder toolFinder)
    {
      this.item = item;
      this.toolFinder = toolFinder;
      toDispose.addElement(this);
    }

    public  void actionPerformed(ActionEvent e)
    {
      toolFinder.open();
    }

    public void dispose()
    {
      item.removeActionListener(this);
    }
  }

  /** Build a tool menu for frame */

  public MaxToolsMenu(String title)
  {
    super(title);
    
    toolFinders = MaxWindowManager.getWindowManager().getToolFinderList();
    toolFinders.addListDataListener(this);

    rebuildToolsMenu();
  }

  private void disposeActionListeners()
  {
    for (int i = 0; i < toDispose.size(); i++)
      ((ToolActionListener) toDispose.elementAt(i)).dispose();

    toDispose.removeAllElements();
  }


  private void rebuildToolsMenu()
  {
    // First, dispose all the action listener (Java VM GC bug !)

    disposeActionListeners();

    MenuItem mi;

    removeAll();

    // Built the tool menu

    for (int i = 0; i < toolFinders.getSize(); i++)
      {
	MaxToolFinder toolFinder;

	toolFinder = (MaxToolFinder) toolFinders.getElementAt(i);

	mi = new MenuItem(toolFinder.getToolName());
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




                  

