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

package ircam.jmax.toolkit.menus;

import java.awt.*;
import java.awt.event.*;

// import javax.swing.*;
import javax.swing.AbstractButton;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.ListModel;


// import javax.swing.event.*;
import javax.swing.event.ListDataEvent;
import javax.swing.event.ListDataListener;
import javax.swing.event.MenuEvent;
import javax.swing.event.MenuListener;


import ircam.jmax.*;

public class MaxWindowJMenu extends JMenu implements ListDataListener 
{
  class WindowMenuListener implements MenuListener
  {
    public void menuSelected(MenuEvent e)
    {
      rebuildWindowMenuIfNeeded();
    }

    public void menuDeselected(MenuEvent e)
    {
    }

    public void menuCanceled(MenuEvent e)
    {
    }
  }

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


  Frame frame;
  int windowOperationCount = -1;
  boolean windowIsActive = false;
  ListModel windowList;
  ListModel toolFinders;
  /** Build a window menu for frame */

  public MaxWindowJMenu(String title, Frame frame)
  {
    super(title);
    setHorizontalTextPosition(AbstractButton.LEFT);
    
    this.frame = frame;
    windowList = MaxWindowManager.getWindowManager().getWindowList();
    toolFinders = MaxWindowManager.getWindowManager().getToolFinderList();
    toolFinders.addListDataListener(this);

    addMenuListener(new WindowMenuListener());
  }

  private void rebuildWindowMenuIfNeeded()
  {
    if (windowOperationCount < MaxWindowManager.getWindowManager().getWindowOperationCount())
      rebuildWindowMenu();
  }

  private void rebuildWindowMenu()
  {
    JMenuItem mi;

    windowOperationCount = MaxWindowManager.getWindowManager().getWindowOperationCount();

    if (getMenuComponentCount() > 0)
      removeAll();

    mi = new JMenuItem("Stack");

    add(mi);
    mi.addActionListener(new ActionListener()
				{public  void actionPerformed(ActionEvent e)
				    { MaxWindowManager.getWindowManager().StackWindows();}});
    mi = new JMenuItem("Tile");
    add(mi);
    mi.addActionListener(new ActionListener()
				{public  void actionPerformed(ActionEvent e)
				    { MaxWindowManager.getWindowManager().TileWindows();}});

    addSeparator();

    for (int i = 0; i < toolFinders.getSize(); i++)
	{
	    MaxToolFinder toolFinder;

	    toolFinder = (MaxToolFinder) toolFinders.getElementAt(i);
	    
	    mi = new JMenuItem(toolFinder.getToolName());
	    if(toolFinder.getToolName().equals("Console"))
		insert(mi, 3);
	    else	    
		add(mi);
	    mi.addActionListener(new ToolActionListener(mi, toolFinder));
	}

    if(windowList.getSize()>0)
	{
	    addSeparator();

	    for (int i = 0; i < windowList.getSize(); i++)
		{
		    final Frame w = (Frame) windowList.getElementAt(i);
		    
		    mi = new JMenuItem(w.getTitle());
		    
		    add(mi);
		    mi.addActionListener(new ActionListener()
			{ public  void actionPerformed(ActionEvent e)
			    { 
				if(w.getState()==Frame.ICONIFIED) w.setState(Frame.NORMAL);
				w.toFront();
			    }
			});
		}
	}
  }

  // ListDataListener
  public void contentsChanged(ListDataEvent e)
  {
    rebuildWindowMenu();
  }

  public void intervalAdded(ListDataEvent e)
  {
    rebuildWindowMenu();
  }

  public void intervalRemoved(ListDataEvent e) 
  {
    rebuildWindowMenu();
  }
}




                  


