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

package ircam.jmax.toolkit.actions;

import java.awt.*;
import java.awt.event.*;

// import javax.swing.*;
import javax.swing.KeyStroke;
import javax.swing.AbstractAction;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.SwingUtilities;

import ircam.jmax.*;
import ircam.jmax.toolkit.*;

abstract public class EditorAction extends AbstractAction
{
  public EditorAction()
  {
    super("");
  }

  public EditorAction(String name)
  {
    super(name);
    this.name = name;
  }

   public EditorAction(String name, String actionCommand, int mnemonic, int keyStroke, boolean enabled)
   {
     super(name);
     putValue(AbstractAction.NAME, name);
     putValue(AbstractAction.ACTION_COMMAND_KEY, actionCommand);

     if(mnemonic != KeyEvent.VK_UNDEFINED)
       putValue(AbstractAction.MNEMONIC_KEY, new Integer(mnemonic));

     if(keyStroke != KeyEvent.VK_UNDEFINED)
       putValue(AbstractAction.ACCELERATOR_KEY, KeyStroke.getKeyStroke(keyStroke, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()));

     setEnabled(enabled);
   }

  public String getName()
  {
    return name;
  }

  public  void actionPerformed(ActionEvent e)
  {
    EditorContainer container = getEditorContainer(e);
    doAction(container);
  }

  EditorContainer getEditorContainer(ActionEvent e){
    Component invoker, menu;

    if(e.getSource() instanceof JMenuItem){
      menu = ((JPopupMenu) ((JMenuItem)e.getSource()).getParent()).getInvoker();
      
      
      while(menu.getParent() instanceof JPopupMenu)
	  menu = ((JPopupMenu) menu.getParent()).getInvoker();

      invoker = menu;
    }
    else
      invoker = (Component)e.getSource();

    return (EditorContainer) SwingUtilities.getAncestorOfClass(EditorContainer.class, invoker);   
  }

  abstract public void doAction(EditorContainer container);
  
  String name;
}

