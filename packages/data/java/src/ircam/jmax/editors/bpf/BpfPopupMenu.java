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

package ircam.jmax.editors.bpf;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class BpfPopupMenu extends JPopupMenu 
{
  BpfEditor target = null;    
  JTextField maxValueField, minValueField;

  public BpfPopupMenu(BpfEditor editor)
  {
    super();
    
    target = editor;

    /////////// Tools /////////////////////////////////////////
    add(target.getToolsMenu());

    addSeparator();    

    /////////////////// list /////////////////////////////////////
    JMenuItem item;
    item = new JMenuItem("View as list");
    item.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	    {
		target.showListDialog();
	    }
    });
    add(item);

    addSeparator();
    ////////////////////// others  //////////////////////////////
    item = new JMenuItem("Select All");
    item.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	    target.getGraphicContext().getSelection().selectAll();
	    target.getGraphicContext().getGraphicDestination().requestFocus();
	}
    });
    add(item);

    addSeparator();

    ////////////////////// Range Menu //////////////////////////////
    JPanel rangePanel = new JPanel();
    rangePanel.setLayout(new BoxLayout(rangePanel, BoxLayout.Y_AXIS));
    
    JLabel rangeLabel = new JLabel("Range", JLabel.CENTER);
    rangeLabel.setForeground(Color.black);

    Box labelRangeBox = new Box(BoxLayout.X_AXIS);
    labelRangeBox.add(Box.createRigidArea(new Dimension(20, 0)));    
    labelRangeBox.add(rangeLabel);    
    labelRangeBox.add(Box.createHorizontalGlue());    

    rangePanel.add(labelRangeBox);    

    JLabel maxLabel = new JLabel("max", JLabel.CENTER);
    maxValueField = new JTextField();
    maxValueField.setPreferredSize(new Dimension(100, 20));
    maxValueField.setMaximumSize(new Dimension(100, 20));
    maxValueField.addActionListener(new ActionListener(){
	    public void actionPerformed( ActionEvent e)
	    {
		float max;		
		try
		    {
			max = Float.valueOf(maxValueField.getText()).floatValue();
			target.getGraphicContext().getDataModel().setMaximumValue(max);
			target.getGraphicContext().getGraphicDestination().repaint();
		    }
		catch (NumberFormatException e1)
		    {
			System.err.println("Error:  invalid number format!");
			return;
		    }
	    }
	});
    
    JPanel maxPanel = new JPanel();
    maxPanel.setPreferredSize(new Dimension(150, 20));
    maxPanel.setLayout(new BoxLayout(maxPanel, BoxLayout.X_AXIS));    
    maxPanel.add(Box.createRigidArea(new Dimension(5, 0)));    
    maxPanel.add(maxLabel);
    maxPanel.add(Box.createHorizontalGlue());    
    maxPanel.add( maxValueField);

    rangePanel.add(maxPanel);

    JLabel minLabel = new JLabel("min", JLabel.CENTER);
    minValueField = new JTextField();
    minValueField.setPreferredSize(new Dimension(100, 20));
    minValueField.setMaximumSize(new Dimension(100, 20));
    minValueField.addActionListener(new ActionListener(){
	    public void actionPerformed( ActionEvent e)
	    {
		float min;		
		try
		    {
			min = Float.valueOf(minValueField.getText()).floatValue();
			target.getGraphicContext().getDataModel().setMinimumValue(min);
			target.getGraphicContext().getGraphicDestination().repaint();
		    }
		catch (NumberFormatException e1)
		    {
			System.err.println("Error:  invalid number format!");
			return;
		    }
	    }
	});

    JPanel minPanel = new JPanel();
    minPanel.setPreferredSize(new Dimension(150, 20));
    minPanel.setLayout(new BoxLayout(minPanel, BoxLayout.X_AXIS));
    minPanel.add(Box.createRigidArea(new Dimension(5, 0)));    
    minPanel.add(minLabel);
    minPanel.add(Box.createHorizontalGlue());    
    minPanel.add( minValueField);

    rangePanel.add(minPanel);
    rangePanel.validate();

    add(rangePanel);

    ///////////////////////////////////
    validate();
    pack();
  }

  public void show(Component invoker, int x, int y)
  {
      update();
      super.show(invoker, x, y);
  }
    
  public void update()
  {
    float min = target.getGraphicContext().getDataModel().getMinimumValue();
    minValueField.setText(""+min);    
    float max = target.getGraphicContext().getDataModel().getMaximumValue();
    maxValueField.setText(""+max);    
    revalidate();
  }
}










