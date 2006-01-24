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

import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

import ircam.jmax.editors.table.*;
import ircam.jmax.editors.table.renderers.*;
import ircam.jmax.editors.table.tools.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class TablePopupMenu extends JPopupMenu 
{
  int x;
  int y;
  TableDisplay target = null;     
  JMenuItem filledItem, pointsItem, linesItem;

  public TablePopupMenu(TableDisplay editor)
  {
    super();
    
    target = editor;
    JMenuItem item;
    item = new JMenuItem("Inspect ... ");
    item.addActionListener(new ActionListener(){
      public void actionPerformed(ActionEvent e)
      {
        TableInspector.inspect(target, target.getFrame(), SwingUtilities.convertPoint(target, x, y, target.getFrame()));
      }
    });
    add(item);
    
    //addSeparator();
    
    /////////// Tools /////////////////////////////////////////
    /*Tool tool;
    ButtonGroup toolsMenuGroup = new ButtonGroup();
    JRadioButtonMenuItem aMenuItem;
    for(Enumeration e = TableTools.instance.getTools(); e.hasMoreElements();)
    {
      tool = (Tool)e.nextElement();
      aMenuItem = new JRadioButtonMenuItem(tool.getName(), tool.getIcon());
      aMenuItem.addActionListener( target.getGraphicContext().getToolManager());
      toolsMenuGroup.add(aMenuItem);
      add(aMenuItem);
    }
    
    ((JRadioButtonMenuItem)getComponent(2)).setSelected(true);*/
    
    //addSeparator();

    /////////////////////// View Menu /////////////////////////////
    /*ButtonGroup viewGroup = new ButtonGroup();

    filledItem = new JRadioButtonMenuItem( "Filled view");
    filledItem.addActionListener( new Actions.FilledViewAction());
    viewGroup.add( filledItem);
    add( filledItem);

    pointsItem = new JRadioButtonMenuItem( "Points view");
    pointsItem.addActionListener( new Actions.PointsViewAction());
    viewGroup.add( pointsItem);
    add( pointsItem);    

    linesItem = new JRadioButtonMenuItem( "Lines view");
    linesItem.addActionListener( new Actions.LinesViewAction());
    viewGroup.add( linesItem);
    add( linesItem);    

    addSeparator();*/
    
    /* Color */
    /*item = new JMenuItem( "Background Color...");
    item.addActionListener( new Actions.BackColorAction());
    add( item);
    
    item = new JMenuItem( "Foreground Color...");
    item.addActionListener( new Actions.ForeColorAction());
    add( item);*/

    validate();
    pack();
  }

  public void show(Component invoker, int x, int y)
  {
    this.x = x;
    this.y = y;
    
    update();
    super.show(invoker, x, y);
  }

  public void update(){}
}











