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

package ircam.jmax.editors.explode.menus;

import javax.swing.*;

import ircam.jmax.editors.explode.*;
import ircam.jmax.editors.explode.actions.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;


public class OptionsMenu extends EditorMenu
{
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
  }

  public void updateMenu()
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



