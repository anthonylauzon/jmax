//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
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




                  

