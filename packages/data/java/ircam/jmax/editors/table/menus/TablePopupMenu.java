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
  JTextField maxValueField;
  JMenuItem solidItem, hollowItem;

  public TablePopupMenu(TableDisplay editor)
  {
    super();
    
    target = editor;

    /////////// Tools /////////////////////////////////////////
    JMenuItem aMenuItem;
    Tool tool;
    ButtonGroup toolsMenuGroup = new ButtonGroup();
    
    for(Enumeration e = TableTools.instance.getTools(); e.hasMoreElements();)
      {
	tool = (Tool)e.nextElement();
	aMenuItem = new JRadioButtonMenuItem(tool.getName(), tool.getIcon());
	aMenuItem.addActionListener( target.getGraphicContext().getToolManager());
	toolsMenuGroup.add(aMenuItem);
	add(aMenuItem);
      }
    
    ((JRadioButtonMenuItem)getComponent(0)).setSelected(true);
    
    addSeparator();

    /////////////////////// View Menu /////////////////////////////
    ButtonGroup viewGroup = new ButtonGroup();

    solidItem = new JRadioButtonMenuItem( "Solid view");
    solidItem.addActionListener( Actions.solidAction);
    viewGroup.add( solidItem);
    add( solidItem);

    hollowItem = new JRadioButtonMenuItem( "Hollow view");
    hollowItem.addActionListener( Actions.hollowAction);
    viewGroup.add( hollowItem);
    add( hollowItem);    
    
    addSeparator();

    ////////////////////// Range Menu //////////////////////////////
    JPanel rangePanel = new JPanel();
    rangePanel.setLayout(new BoxLayout(rangePanel, BoxLayout.Y_AXIS));
    
    JLabel rangeLabel = new JLabel("Vertical Range", JLabel.CENTER);
    rangeLabel.setForeground(Color.black);

    Box labelRangeBox = new Box(BoxLayout.X_AXIS);
    labelRangeBox.add(Box.createRigidArea(new Dimension(20, 0)));    
    labelRangeBox.add(rangeLabel);    
    labelRangeBox.add(Box.createHorizontalGlue());    

    rangePanel.add(labelRangeBox);    

    ActionListener rangeListener = new ActionListener(){
	public void actionPerformed( ActionEvent e)
	{
	  setRange();
	}
      };

    JLabel maxLabel = new JLabel("max", JLabel.CENTER);
    maxValueField = new JTextField();
    maxValueField.setPreferredSize(new Dimension(100, 20));
    maxValueField.setMaximumSize(new Dimension(100, 20));
    maxValueField.addActionListener(rangeListener);
    
    JPanel maxPanel = new JPanel();
    maxPanel.setPreferredSize(new Dimension(150, 20));
    maxPanel.setLayout(new BoxLayout(maxPanel, BoxLayout.X_AXIS));    
    maxPanel.add(Box.createRigidArea(new Dimension(5, 0)));    
    maxPanel.add(maxLabel);
    maxPanel.add(Box.createHorizontalGlue());    
    maxPanel.add( maxValueField);

    rangePanel.add(maxPanel);

    /*JLabel minLabel = new JLabel("min", JLabel.CENTER);
      minValueField = new JTextField();
      minValueField.setPreferredSize(new Dimension(100, 20));
      minValueField.setMaximumSize(new Dimension(100, 20));
      minValueField.addActionListener(rangeListener);
      
      JPanel minPanel = new JPanel();
      minPanel.setPreferredSize(new Dimension(150, 20));
      minPanel.setLayout(new BoxLayout(minPanel, BoxLayout.X_AXIS));
      minPanel.add(Box.createRigidArea(new Dimension(5, 0)));    
      minPanel.add(minLabel);
      minPanel.add(Box.createHorizontalGlue());    
      minPanel.add( minValueField);

      rangePanel.add(minPanel);*/
    rangePanel.validate();

    add(rangePanel);

    ///////////////////////////////////
    addPopupMenuListener(new PopupMenuListener(){
	public void popupMenuWillBecomeVisible(PopupMenuEvent e){}
	public void popupMenuWillBecomeInvisible(PopupMenuEvent e)
	{
	  setRange();
	}
	public void popupMenuCanceled(PopupMenuEvent e){}
      });

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

  public void setRange()
  {
    try
      {
	int max = Integer.valueOf( maxValueField.getText()).intValue();
	target.panel.setMaximumValue(max);
      }
    catch (NumberFormatException e1)
      {
	System.err.println("Error:  invalid number format!");
	return;
      }
  }

  public void update()
  {
    int dm = target.getDisplayMode();
    if( dm == TableRenderer.SOLID) solidItem.setSelected( true);
    else hollowItem.setSelected( true);
    
    int max = target.getGraphicContext().getVerticalMaximum();
    maxValueField.setText(""+max);    
    revalidate();
  }
}











