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

package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.editors.patcher.*;

/** An utility class that provide a constructor to make
  actions suitable to be put in an ErmesSketchWindow sub menu (menu inside a menu).
  */

abstract public class SubMenuAction implements ActionListener
{
  public SubMenuAction()
  {
  }

  public  void actionPerformed(ActionEvent e)
  {
    Component menu;
    Component invoker;
    ErmesSketchWindow editor;

    menu    = ((JPopupMenu) ((JMenuItem)e.getSource()).getParent()).getInvoker();
    invoker = ((JPopupMenu) menu.getParent()).getInvoker();

    editor = (ErmesSketchWindow) SwingUtilities.getAncestorOfClass(ErmesSketchWindow.class,
								   invoker);
    doAction(editor);
  }

  abstract public void doAction(ErmesSketchWindow editor);
}


