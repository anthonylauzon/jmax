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
    String descr;
    AddPopUp popup;

    AddPopUpListener(String descr, AddPopUp popup)
    {
      this.descr = descr;
      this.popup = popup;
    }

    public void actionPerformed( ActionEvent e)
    {
      Component invoker;
      
      invoker = ((JPopupMenu) ((JMenuItem)e.getSource()).getParent()).getInvoker();
      ((ErmesSketchPad) invoker).makeObject(descr, popup.x, popup.y);
    }
  };

  static class AddPopUpSubMenuListener implements ActionListener
  {

    String descr;
    AddPopUp popup;

    AddPopUpSubMenuListener(String descr, AddPopUp popup)
    {
      this.descr = descr;
      this.popup = popup;
    }

    public void actionPerformed( ActionEvent e)
    {
      Component menu;
      Component invoker;
      ErmesSketchPad editor;

      menu    = ((JPopupMenu) ((JMenuItem)e.getSource()).getParent()).getInvoker();
      invoker = ((JPopupMenu) menu.getParent()).getInvoker();

      ((ErmesSketchPad) invoker).setAddModeInteraction(descr);
      // ((ErmesSketchPad) invoker).makeObject(descr, popup.x, popup.y);
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


  static public void addAbbreviation(String cmd, String descr)
  {
    JMenuItem item;

    if ((cmd.charAt(0) == '%') || (cmd.charAt(0) == '_'))
      item = new JMenuItem(Icons.get(cmd));
    else
      item = new JMenuItem(cmd);

    item.addActionListener(new AddPopUpListener(descr, popup));
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

  static public void addAbbreviation(String menuName, String cmd, String descr)
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

    item.addActionListener(new AddPopUpSubMenuListener(descr, popup));
    menu.add(item);
  }
}



