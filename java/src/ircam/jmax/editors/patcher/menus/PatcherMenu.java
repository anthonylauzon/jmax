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

package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;

// import javax.swing.*;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.KeyStroke;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.actions.*;

/**
 * Utility to build menus; note that for some unknow reason or bug,
 * Menu installed with Actions do not work after a while, usually after
 * a New followed by the use of a Menu on an old window. 
 * So we use standard actionlistener programming.
 */

abstract class PatcherMenu extends JMenu
{
  PatcherMenu(String name)
  {
    super(name);
    setDelay(0); // ?? Usefull ?
  }

  public JMenuItem add(ActionListener action, String name, int modifiers, int mnemonic)
  {
    JMenuItem item;

    item = new JMenuItem(name, mnemonic);
    item.addActionListener(action);
    item.setAccelerator(KeyStroke.getKeyStroke(mnemonic, modifiers));

    add(item);

    return item;
  }

  public JMenuItem add(ActionListener action, String name)
  {
    JMenuItem item;

    item = new JMenuItem(name);
    item.addActionListener(action);
    add(item);

    return item;
  }
}






