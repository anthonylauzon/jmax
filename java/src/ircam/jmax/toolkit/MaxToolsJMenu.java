package ircam.jmax.toolkit;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;

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
    
    toolFinders = MaxWindowManager.getWindowManager().getToolFinderList();
    toolFinders.addListDataListener(this);

    rebuildToolsMenu();
  }

  public void removeNotify()
  {
    toolFinders.removeListDataListener(this);
    super.removeNotify();
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




                  

