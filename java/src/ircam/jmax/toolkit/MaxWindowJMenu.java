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

public class MaxWindowJMenu extends JMenu implements WindowListener, ListDataListener 
{
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

    windowList.addListDataListener(this);
    frame.addWindowListener(this);

    rebuildWindowMenu();
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

  // WindowListener interface

  public void windowClosing(WindowEvent e)
  {
  }

  public void windowOpened(WindowEvent e)
  {
    rebuildWindowMenuIfNeeded();
  }

  public void windowClosed(WindowEvent e)
  {
  }

  public void windowIconified(WindowEvent e)
  {
  }       

  public void windowDeiconified(WindowEvent e)
  {
    rebuildWindowMenuIfNeeded();
  }

  public void windowActivated(WindowEvent e)
  {
    windowIsActive = true;
    rebuildWindowMenuIfNeeded();
  }

  public void windowDeactivated(WindowEvent e)
  {
    windowIsActive = false;
  }

  // ListDataListener

  public void contentsChanged(ListDataEvent e)
  {
    if (windowIsActive)
      rebuildWindowMenu();
  }

  public void intervalAdded(ListDataEvent e)
  {
    if (windowIsActive)
      rebuildWindowMenuIfNeeded();
  }

  public void intervalRemoved(ListDataEvent e) 
  {
    if (windowIsActive)
      rebuildWindowMenuIfNeeded();
  }
}




                  

