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

import java.io.*;

import java.awt.*;
import java.awt.event.*;

// import javax.swing.*;
import javax.swing.AbstractButton;
import javax.swing.JMenuItem;

import ircam.jmax.*;
import ircam.jmax.editors.project.*;
import ircam.jmax.editors.configuration.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

/** Implement the patcher editor File Menu */

public class DefaultProjectMenu extends EditorMenu
{
  public DefaultProjectMenu()
  {
    this( null);
  }

  public DefaultProjectMenu( EditorContainer editor)
  {
    super("Project");
	setMnemonic(KeyEvent.VK_P);

    setHorizontalTextPosition(AbstractButton.LEFT);
    setDefaultNumEntries(8);

    add(DefaultActions.newProjectAction); // , "New Project");
    add(DefaultActions.openProjectAction); // , "Open Project ...");
    add(DefaultActions.editCurrentProjectAction); //, "Edit Current Project");
    if (editor instanceof ProjectEditor)
	   add(DefaultActions.saveAsDefaultProjectAction); //, "Save As Default Project");
    addSeparator();
    add(DefaultActions.newPackageAction); // , "New Package");
    add(DefaultActions.openPackageAction); //, "Open Package ...");
    addSeparator();
    add(DefaultActions.newConfigurationAction); //, "New Configuration");
    add(DefaultActions.openConfigurationAction); //, "Open Configuration ...");
    add(DefaultActions.editCurrentConfigurationAction); //, "Edit Current Configuration");
    if (editor instanceof ConfigurationEditor)
	   add(DefaultActions.saveAsDefaultConfigurationAction); //, "Save As Default Configuration");
  }
}

