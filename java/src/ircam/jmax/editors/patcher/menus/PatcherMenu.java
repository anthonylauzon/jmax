package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

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






