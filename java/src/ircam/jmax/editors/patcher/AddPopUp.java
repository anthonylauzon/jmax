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

package ircam.jmax.editors.patcher;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.toolkit.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class AddPopUp extends JPopupMenu
{
  static private AddPopUp popup = new AddPopUp();
  static private Hashtable subMenus = new Hashtable();

  int x;
  int y;

  static class AddPopUpListener implements ActionListener
  {
    boolean edit;
    String descr;
    String message;

    AddPopUpListener(String descr, String message, boolean edit)
    {
      this.descr = descr;
      this.message = message;
      this.edit = edit;
    }

    public void actionPerformed( ActionEvent e)
    {
      Component invoker;
      
      invoker = ((JPopupMenu) ((JMenuItem)e.getSource()).getParent()).getInvoker();
      ((ErmesSketchPad) invoker).setAddModeInteraction(descr, message, edit);
    }
  };

  static class AddPopUpSubMenuListener implements ActionListener
  {
    boolean edit;
    String descr;
    String message;

    AddPopUpSubMenuListener(String descr, String message, boolean edit)
    {
      this.descr = descr;
      this.message = message;
      this.edit  = edit;
    }

    public void actionPerformed( ActionEvent e)
    {
      Component menu;
      Component invoker;
      ErmesSketchPad editor;

      menu    = ((JPopupMenu) ((JMenuItem)e.getSource()).getParent()).getInvoker();
      invoker = ((JPopupMenu) menu.getParent()).getInvoker();

      ((ErmesSketchPad) invoker).setAddModeInteraction(descr, message, edit);
    }
  };

  //
  // Constructor accepting the number of in/out to show in the popup
  //

  public AddPopUp()
  {
  }

  static public void popup(Component invoker, int x, int y)
  {
    popup.x = x;
    popup.y = y;
    popup.show(invoker, x - 2, y - 2);
  }

  static public void addAbbreviation(String cmd, String descr, String message, boolean edit)
  {
    JMenuItem item;

    if ((cmd.charAt(0) == '%') || (cmd.charAt(0) == '_'))
      item = new JMenuItem(Icons.get(cmd));
    else
      item = new JMenuItem(cmd);

    item.addActionListener(new AddPopUpListener(descr, message, edit));
    popup.add(item);
  }


  static JMenu addAbbreviationMenu(String name)
  {
    JMenu menu;

    if (name.charAt(0) == '%')
      {
	menu = new JMenu();
	menu.setIcon(Icons.get(name));
      }
    else
      menu = new JMenu(name);

    popup.add(menu);
    subMenus.put(name, menu);

    return menu;
  }

  static public void addAbbreviation(String menuName, String cmd, String descr, String message, boolean edit)
  {
    JMenuItem item;
    JMenu menu;

    menu = (JMenu) subMenus.get(menuName);

    if (menu == null)
      menu = addAbbreviationMenu(menuName);
    
    if ((cmd.charAt(0) == '%') || (cmd.charAt(0) == '_'))
      item = new JMenuItem(Icons.get(cmd));
    else
      item = new JMenuItem(cmd);

    item.addActionListener(new AddPopUpSubMenuListener(descr, message, edit));
    menu.add(item);
  }
}



