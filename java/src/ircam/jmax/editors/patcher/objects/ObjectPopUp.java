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

package ircam.jmax.editors.patcher.objects;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.actions.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class ObjectPopUp extends JPopupMenu implements PopupMenuListener
{
  static private ObjectPopUp popup = new ObjectPopUp();
  private Hashtable subMenus = new Hashtable();
  private Hashtable items = new Hashtable();
  public JMenuItem removeMenuItem;

  int x;
  int y;
  GraphicObject target = null;

  public ObjectPopUp()
  {
    super();
    removeMenuItem = new JMenuItem("Remove Connections");
    removeMenuItem.addActionListener(Actions.removeConnectionsAction);

    add(Actions.bringToFrontObjectAction, "Bring To Front");
    add(Actions.sendToBackObjectAction, "Send To Back");
    //add(Actions.inspectObjectAction, "Inspect");
    pack();

    addPopupMenuListener(this);
  }

  boolean separation = false;
  static public void addSeparation()
  {
    if(popup.inOutItemAdded)
      popup.insert(new Separator(), 3);
    else
      popup.insert(new Separator(), 2);
    popup.separation = true;
  }
  static public void removeSeparation()
  {
      if(popup.separation)
      {
	if(popup.inOutItemAdded)
	  popup.remove(3);
	else
	  popup.remove(2);
	      
	popup.separation = false;
      }
  }
 
  boolean inOutItemAdded = false;
  static public void update(int where, String text)
  {
      popup.removeMenuItem.setText(text);
      switch(where)
	  {
	  case GraphicObject.ON_INLET:	      
	  case GraphicObject.ON_OUTLET:
	    if(!popup.inOutItemAdded)
	      {
		popup.add(popup.removeMenuItem, 0);
		popup.inOutItemAdded = true;
	      }
	    break;
	  case GraphicObject.ON_OBJECT:
	  default:
	    if(popup.inOutItemAdded)
	      {
		popup.remove(0);
		popup.inOutItemAdded = false;
	      }
	    break;
	  }
      popup.revalidate();
      popup.pack();
  }

  static public GraphicObject getPopUpTarget(){
    return popup.target;
  }
  static public ObjectPopUp getInstance(){
    return popup;
  }
  public static JMenuItem getMenuItem(String name)
  {
    return (JMenuItem) popup.items.get(name);
  }

  static public void popup(Component invoker, GraphicObject object, int x, int y)
  {
      popup.target = object;
      popup.x = x;
      popup.y = y;
   
      popup.setPopupSize(100, 200);
      popup.show(invoker, x - 2, y - 2);
  }

  public JMenuItem add(EditorAction action, String name)
  {
    JMenuItem item;

    item = new JMenuItem(name);
    item.addActionListener(action);
    items.put(name, item);
    add(item);
    pack();

    return item;
  }

  public static JMenuItem addItem(EditorAction action, String name)
  {
    JMenuItem item;

    item = new JMenuItem(name);
    item.addActionListener(action);
    popup.items.put(name, item);
    popup.add(item);
    popup.pack();

    return item;
  }
  
  public static void addItem(JMenuItem item)
  {
    popup.add(item);
    popup.items.put(item.getText(), item);
    popup.pack();
  }
  public static JMenuItem addItem(EditorAction action, String name, String menuName)
  {
    JMenuItem item;
    JMenu menu;

    menu = (JMenu) popup.subMenus.get(menuName);
    if (menu == null)
      menu = addMenu(menuName);

    item = new JMenuItem(name);
    item.addActionListener(action);
    menu.add(item);

    return item;
  }

  public static JMenu addMenu(String name)
  {
    JMenu menu;

//      if (name.charAt(0) == '%')
//        {
//  	menu = new JMenu();
//  	menu.setIcon( Icons.get(name));
//        }
//      else
      menu = new JMenu(name);

    popup.add(menu);
    popup.pack();
    popup.subMenus.put(name, menu);

    return menu;
  }
  
  public static void addMenu(JMenu menu)
  {
    popup.add(menu);
    popup.pack();
    popup.subMenus.put(menu.getText(), menu);
  }

  public static void removeMenu(JMenu menu)
  {
    popup.remove(menu);
    popup.pack();
    popup.subMenus.remove(menu.getText());
  }
  public static void removeItem(JMenuItem item)
  {
    popup.remove(item);
    popup.items.remove(item.getText());
    popup.pack();
  }
  
  //PopupMenuListener interface
  public void popupMenuCanceled(PopupMenuEvent e){}
  public void popupMenuWillBecomeInvisible(PopupMenuEvent e)
  {
      target.popUpReset();
  } 
  public void popupMenuWillBecomeVisible(PopupMenuEvent e)
  {
  } 
}











