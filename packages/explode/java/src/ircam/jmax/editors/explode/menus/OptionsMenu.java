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

package ircam.jmax.editors.explode.menus;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;

import ircam.jmax.editors.explode.*;
import ircam.jmax.editors.explode.actions.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;


public class OptionsMenu extends EditorMenu
{
  class OptionsMenuListener implements MenuListener
  {
    public void menuSelected(MenuEvent e){
      updateMenu();
    }
    public void menuDeselected(MenuEvent e){}
    public void menuCanceled(MenuEvent e){}
  }

  JMenuItem settingsItem;
  JMenuItem viewItem;

  EditorContainer container;
  
  public OptionsMenu(EditorContainer container)
  {
    super("Options");

    this.container = container;

    setHorizontalTextPosition(AbstractButton.LEFT);

    settingsItem          = add(Actions.settingsAction, "Settings...");
    viewItem = add(Actions.viewAction, "Table view");

    addMenuListener(new OptionsMenuListener());
  }

  private void updateMenu()
  {
    if(((ExplodePanel)container.getEditor()).getCurrentView()==ExplodePanel.PIANOROLL_VIEW){
      viewItem.setText("Table View");
      settingsItem.setEnabled(true);
    }
    else{
      viewItem.setText("PianoRoll View");
      settingsItem.setEnabled(false);
    }
  }
}



