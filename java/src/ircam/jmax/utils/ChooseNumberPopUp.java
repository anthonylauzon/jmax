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

package ircam.jmax.utils;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class ChooseNumberPopUp extends JPopupMenu
{
  private static ChooseNumberPopUp  popup = new ChooseNumberPopUp();

  private NumberChoosenListener listener = null;
  private int max = 0;
  private int min = 0;

  class ChooseNumberPopUpActionListener implements ActionListener
  {
    int idx;

    ChooseNumberPopUpActionListener(int idx)
    {
      this.idx = idx;
    }

    public void actionPerformed( ActionEvent e)
    { 
      listener.numberChoosen( idx);
    }
  };
    
  private void redefine(int min, int max)
  {
    JMenuItem aMenuItem;

    if ((min == this.min) && (max == this.max))
      return;
    
    // Remove all the components

    for (int i = 0; i < (this.max - this.min); i++)
      remove(getComponentAtIndex(0));

    // Rebuild the menu

    this.min = min;
    this.max = max;

    for (int i = min; i < max; i++)
      {
	aMenuItem = new JMenuItem( Integer.toString( i));

	add(aMenuItem);
	aMenuItem.addActionListener(new ChooseNumberPopUpActionListener(i));
      }
  }

  //
  // Constructor
  //

  public ChooseNumberPopUp() 
  {
    super();
  }

  public static void choose(Component invoker, NumberChoosenListener listener, int min, int max, Point p)
  {
    popup.listener = listener;
    popup.redefine(min, max);
    popup.show(invoker, p.x - 2, p.y - 2);
  }
}



