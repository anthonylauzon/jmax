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

package ircam.jmax.editors.table.menus;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;
import ircam.jmax.editors.table.*;
import ircam.jmax.editors.table.actions.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class TablePopupMenu extends JPopupMenu 
{
  int x;
  int y;
  TableDisplay target = null;     

  public TablePopupMenu(TableDisplay editor)
  {
    super();
    
    target = editor;

    add(target.panel.toolbar.itsMenu);

    addSeparator();

    JMenu viewMenu = new JMenu("Change View");
    JMenuItem item = new JMenuItem("Hollow view");
    item.addActionListener(Actions.hollowAction);
    viewMenu.add(item);    
    item = new JMenuItem("Solid view");
    item.addActionListener(Actions.solidAction);
    viewMenu.add(item);
    add(viewMenu);
    
    addSeparator();

    item = new JMenuItem("Change Range");
    item.addActionListener(new ActionListener(){
	    public void actionPerformed(ActionEvent e)
	    {
		ChangeRangeDialog.changeRange(target.getGraphicContext().getFrame(),	
					      target.getGraphicContext(), 
					      SwingUtilities.convertPoint(target, x, y,
									  target.getGraphicContext().getFrame()),
					      target.panel);
	    }
	});
    add(item);

    pack();
  }

  public void show(Component invoker, int x, int y)
  {
      this.x = x;
      this.y = y;
      
      super.show(invoker, x, y);
  }
}











