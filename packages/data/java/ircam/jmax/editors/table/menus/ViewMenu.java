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

package ircam.jmax.editors.table.menus;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

import ircam.jmax.editors.table.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;
import ircam.jmax.toolkit.actions.*;

public class ViewMenu extends EditorMenu
{
  public EditorAction pointsAction = new Actions.PointsViewAction();
  public EditorAction filledAction = new Actions.FilledViewAction();
  public EditorAction linesAction = new Actions.LinesViewAction();
  public EditorAction backAction = new Actions.BackColorAction();
  public EditorAction foreAction = new Actions.ForeColorAction();
  public EditorAction zoomAction = new Actions.ZoomToWindowAction();

  EditorContainer container;
  
  public ViewMenu(EditorContainer container)
  {
    super("View");

    this.container = container;

    setHorizontalTextPosition(AbstractButton.LEFT);

    add(pointsAction);
    add(filledAction);
    add(linesAction);
    
    addSeparator();
    
    add(backAction);
    add(foreAction);
    
    addSeparator();
    
    add(zoomAction);
  }
}


