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

package ircam.jmax.toolkit;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;

public class MaxWindowMenu extends Menu implements WindowListener, ListDataListener 
{
  Frame frame;
  int windowOperationCount = -1;
  boolean windowIsActive = false;
  ListModel windowList;
  private MaxVector toDispose = new MaxVector();

  abstract class WindowActionListener implements ActionListener
  {
    MenuItem item;

    WindowActionListener(MenuItem item)
    {
      this.item = item;
      toDispose.addElement(this);
    }

    public  abstract void actionPerformed(ActionEvent e);

    public void dispose()
    {
      item.removeActionListener(this);
    }
  }

  /** Build a window menu for frame */

  public MaxWindowMenu(String title, Frame frame)
  {
    super(title);
    
    this.frame = frame;
    windowList = MaxWindowManager.getWindowManager().getWindowList();

    windowList.addListDataListener(this);
    frame.addWindowListener(this);

    rebuildWindowMenu();
  }

  private void disposeActionListeners()
  {
    for (int i = 0; i < toDispose.size(); i++)
      ((WindowActionListener) toDispose.elementAt(i)).dispose();

    toDispose.removeAllElements();
  }

  private void rebuildWindowMenuIfNeeded()
  {
    if (windowOperationCount < MaxWindowManager.getWindowManager().getWindowOperationCount())
      rebuildWindowMenu();
  }

  private void rebuildWindowMenu()
  {
    // First, dispose all the action listener (Java VM GC bug !)

    disposeActionListeners();

    MenuItem mi;

    windowOperationCount = MaxWindowManager.getWindowManager().getWindowOperationCount();
    removeAll();

    mi = new MenuItem("Stack");
    add(mi);
    mi.addActionListener(new WindowActionListener(mi)
				{public  void actionPerformed(ActionEvent e)
				    { MaxWindowManager.getWindowManager().StackWindows();}});

    mi = new MenuItem("Tile");
    add(mi);
    mi.addActionListener(new WindowActionListener(mi)
				{public  void actionPerformed(ActionEvent e)
				    { MaxWindowManager.getWindowManager().TileWindows();}});

    addSeparator();

    for (int i = 0; i < windowList.getSize(); i++)
      {
	final Frame w = (Frame) windowList.getElementAt(i);

	mi = new MenuItem(w.getTitle());
	add(mi);
	mi.addActionListener(new WindowActionListener(mi)
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




                  

