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

package ircam.jmax.editors.sequence.menus;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.actions.*;
import ircam.jmax.editors.sequence.track.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class ListPopupMenu extends JPopupMenu
{
  static private ListPopupMenu popup = new ListPopupMenu();

  int x;
  int y;
  ListPanel target = null;    

  public ListPopupMenu()
  {
    super();
    
    JMenuItem item;
    item = new JMenuItem("Remove Selection");
    item.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	    {
		ListPopupMenu.getPopupTarget().removeSelection();
	    }
    });
    add(item);    
    item = new JMenuItem("Add After");
    item.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	    {
		ListPopupMenu.getPopupTarget().addAfter();
	    }
    });
    add(item);

    pack();
  }

  static public ListPopupMenu getInstance()
  {
    return popup;
  }

  static public ListPanel getPopupTarget(){
    return popup.target;
  }

  static public void update(ListPanel panel)
  {
    popup.target = panel;
  }
}











