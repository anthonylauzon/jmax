//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.patcher;

import java.util.*;
import java.awt.*;
// import javax.swing.*;
import javax.swing.BorderFactory;
import javax.swing.JMenu;
import javax.swing.JMenuBar;


import ircam.jmax.toolkit.*;
import ircam.jmax.*;

//
// The pop-up button used to display object-buttons in the toolbar when too long
//

public class MoreObjectsButton extends JMenuBar
{
  private JMenu menu;  

  public MoreObjectsButton(Enumeration buttons)
  {
      super();
      setMargin(new Insets(0, 0, 0, 0));
      setBorder(BorderFactory.createEmptyBorder());
      setPreferredSize(new Dimension(24, 20));
      setMinimumSize(new Dimension(24, 20));

      menu = new JMenu("");
      menu.setMargin(new Insets(0, 0, 0, 0));
      menu.setIcon( JMaxIcons.moreObjects);
      menu.setPreferredSize(new Dimension(24, 19));
      menu.setMinimumSize(new Dimension(24, 19));

      for(Enumeration e = buttons; e.hasMoreElements();)
	  menu.add((ToolButton)e.nextElement(), 0);
      add(menu);
  }

  public void update(Enumeration buttons, int size)
  {
      ToolButton button;
      int count = menu.getItemCount();
      if(size == count) return;
      if(size>count)
	  {
	      int i=0;
	      for(Enumeration e = buttons; e.hasMoreElements();)
		  {
		      button = (ToolButton)e.nextElement();

		      if(i++ >= count)
			menu.add(button, 0);
		  }
	  }
      else
	  for(int i=0; i<count-size; i++)
	      menu.remove(0);
  }
}











