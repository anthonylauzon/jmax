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

package ircam.jmax.guiobj;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

//import javax.swing.*;
import javax.swing.JPopupMenu;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

import ircam.jmax.editors.patcher.objects.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class MatDispRModePopUp extends JPopupMenu
{
  static public MatDispRModePopUp popup = new MatDispRModePopUp();

  GraphicObject target = null;
  MatDispControlPanel controlPanel;    

  public MatDispRModePopUp()
  {
    super();

    controlPanel = new MatDispControlPanel();
    add(controlPanel);
    validate();

    pack();
  }

  static public GraphicObject getPopUpTarget(){
    return popup.target;
  }

  public static void update(GraphicObject obj)
  {
    popup.target = obj;
    popup.controlPanel.update(obj);    
  }
}











