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

package ircam.jmax.utils;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class ChooseColorIdPopUp extends JPopupMenu
{
  private static ChooseColorIdPopUp  popup = null;
  private NumberChoosenListener listener = null;

  // Do not change this array; the indexes in this array are stored
  // in patches; change in this array will produce a change
  // in stored patches.

  static Color colors[] = 
  {
    Color.yellow,
    Color.blue,
    Color.cyan,
    Color.green,
    Color.magenta,
    Color.orange,
    Color.pink,
    Color.red,
    Color.white,
    Color.black
  };

  class ChooseColorIdUpActionListener implements ActionListener
  {
    int idx;

    ChooseColorIdUpActionListener(int idx)
    {
      this.idx = idx;
    }

    public void actionPerformed( ActionEvent e)
    {
      listener.numberChoosen( idx);
    }
  };
    
  public ChooseColorIdPopUp() 
  {
    super();

    // build the menu

    for (int i = 0; i < colors.length; i++)
      {
	JMenuItem aMenuItem = new JMenuItem( "   ");
	aMenuItem.setBackground(colors[i]);
	aMenuItem.setForeground(colors[i]);
	aMenuItem.setFocusPainted(false);
	add(aMenuItem);
 	aMenuItem.addActionListener(new ChooseColorIdUpActionListener(i));
      }
  }

  public static void choose(Component invoker, NumberChoosenListener listener, Point p)
  {
    if (popup == null)
      popup = new ChooseColorIdPopUp();

    popup.listener = listener;
    popup.show(invoker, p.x - 2, p.y - 2);
  }
}



