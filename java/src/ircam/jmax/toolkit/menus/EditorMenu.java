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

package ircam.jmax.toolkit.menus;

import java.awt.*;
import java.awt.event.*;

// import javax.swing.*;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.KeyStroke;

// import javax.swing.event.*;
import javax.swing.event.MenuEvent;
import javax.swing.event.MenuListener;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

/**
 * Utility to build menus; note that for some unknow reason or bug,
 * Menu installed with Actions do not work after a while, usually after
 * a New followed by the use of a Menu on an old window. 
 * So we use standard actionlistener programming.
 */

public abstract class EditorMenu extends JMenu					 
{
  public EditorMenu(String name)
  {
    super(name);
    setDelay(0); // ?? Usefull ?

    addMenuListener(new MenuListener(){
	public void menuSelected(MenuEvent e)
	{
	   updateMenu();
	}
	
	public void menuDeselected(MenuEvent e)
	{
	}
	
	public void menuCanceled(MenuEvent e)
	{
	}
      });
  }

  public int DEFAULT_NUM_ENTRIES;
  public void setDefaultNumEntries(int n)
  {
      DEFAULT_NUM_ENTRIES = n;
  }
  public int getDefaultNumEntries()
  {
      return DEFAULT_NUM_ENTRIES;
  }

  public JMenuItem add(EditorAction action, String name, int modifiers, int mnemonic)
  {
    JMenuItem item;

    item = new JMenuItem(action);

    // item.addActionListener(action);
	// item.setAction(action);
    // item.setAccelerator(KeyStroke.getKeyStroke(mnemonic, modifiers));

    add(item);

    return item;
  }

  public JMenuItem insert(EditorAction action, String name, int modifiers, int mnemonic, int position)
  {
    JMenuItem item;

    item = new JMenuItem(action); 
	// item.addActionListener(action);
	// item.setAction(action);
    // item.setAccelerator(KeyStroke.getKeyStroke(mnemonic, modifiers));

    insert(item, position);

    return item;
  }

  public JMenuItem add(EditorAction action, String name)
  {
    JMenuItem item;

    item = new JMenuItem(action); 
	// item.addActionListener(action);
	// item.setAction(action);
    add(action);

    return item;
  }

  public JMenuItem replace(EditorAction action, String name, int position)
  {
    JMenuItem item;
    
    item = new JMenuItem(name); 
	// item.addActionListener(action);
	item.setAction(action);
    remove( position);
    insert( item, position); 
    
    return item;
  }
  
  public JCheckBoxMenuItem addCheck(EditorAction action, String name)
  {
    JCheckBoxMenuItem item;

    item = new JCheckBoxMenuItem(name);
	// item.addActionListener(action);
	item.setAction(action);
    add(item);

    return item;
  }

  public JMenuItem insert(EditorAction action, String name, int position)
  {
    JMenuItem item;

    item = new JMenuItem(name);
	// item.addActionListener(action);
	item.setAction(action);
    insert(item, position);

    return item;
  } 
    
  public void setEnabled(boolean enabled, int position)
  {
    getItem( position).setEnabled( enabled);
  }

  public void updateMenu(){};
}






