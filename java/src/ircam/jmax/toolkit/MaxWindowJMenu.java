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




                  

