//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
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

public class MaxWindowJMenu extends JMenu
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

  Frame frame;
  int windowOperationCount = -1;
  boolean windowIsActive = false;
  ListModel windowList;

  /** Build a window menu for frame */

  public MaxWindowJMenu(String title, Frame frame)
  {
    super(title);
    setHorizontalTextPosition(AbstractButton.LEFT);
    
    this.frame = frame;
    windowList = MaxWindowManager.getWindowManager().getWindowList();

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

    for (int i = 0; i < windowList.getSize(); i++)
      {
	final Frame w = (Frame) windowList.getElementAt(i);

	mi = new JMenuItem(w.getTitle());
	add(mi);
	mi.addActionListener(new ActionListener()
			     { public  void actionPerformed(ActionEvent e)
			       { w.toFront();}});
      }
  }
}




                  

