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

package ircam.jmax.guiobj;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;
import ircam.jmax.widgets.*;

import ircam.jmax.editors.patcher.objects.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class VecDispControlPanel extends JPanel implements ActionListener, ObjectControlPanel
{
  GraphicObject target = null;
  JTextField maxValueField, minValueField;
  float min, max;

  public VecDispControlPanel()
  {
    super();
    setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

    JLabel titleLabel = new JLabel("Range", JLabel.CENTER);
    titleLabel.setForeground(Color.black);

    Box labelBox = new Box(BoxLayout.X_AXIS);
    labelBox.add(Box.createRigidArea(new Dimension(20, 0)));    
    labelBox.add(titleLabel);    
    labelBox.add(Box.createHorizontalGlue());    

    add(labelBox);    

    JLabel maxLabel = new JLabel("max", JLabel.CENTER);
    maxValueField = new JTextField();
    maxValueField.setPreferredSize(new Dimension(100, 20));
    maxValueField.setMaximumSize(new Dimension(100, 20));
    maxValueField.addActionListener(this);
    
    JPanel maxPanel = new JPanel();
    maxPanel.setPreferredSize(new Dimension(150, 20));
    maxPanel.setLayout(new BoxLayout(maxPanel, BoxLayout.X_AXIS));    
    maxPanel.add(Box.createRigidArea(new Dimension(5, 0)));    
    maxPanel.add(maxLabel);
    maxPanel.add(Box.createHorizontalGlue());    
    maxPanel.add( maxValueField);
    maxPanel.add(new IncrementController(new IncrementListener(){
	public void increment()
	{
	  max++;
	  maxValueField.setText(""+max);    
	}
	public void decrement()
	{
	  max--;
	  maxValueField.setText(""+max);    
	}
      }));
    maxPanel.validate(); 

    add(maxPanel);

    JLabel minLabel = new JLabel("min", JLabel.CENTER);
    minValueField = new JTextField();
    minValueField.setPreferredSize(new Dimension(100, 20));
    minValueField.setMaximumSize(new Dimension(100, 20));
    minValueField.addActionListener(this);
    
    JPanel minPanel = new JPanel();
    minPanel.setPreferredSize(new Dimension(150, 20));
    minPanel.setLayout(new BoxLayout(minPanel, BoxLayout.X_AXIS));
    minPanel.add(Box.createRigidArea(new Dimension(5, 0)));    
    minPanel.add(minLabel);
    minPanel.add(Box.createHorizontalGlue());    
    minPanel.add( minValueField);
    minPanel.add(new IncrementController(new IncrementListener(){
	public void increment()
	{
	  min++;
	  minValueField.setText(""+min);    
	}
	public void decrement()
	{
	  min--;
	  minValueField.setText(""+min);    
	}
      }));    
    minPanel.validate();

    add(minPanel);

    validate();
  }

  public void update(GraphicObject obj)
  {
    target = obj;
    min = ((FtsVectorDisplayObject)obj.getFtsObject()).getMinimum();
    minValueField.setText(""+min);    
    max = ((FtsVectorDisplayObject)obj.getFtsObject()).getMaximum();
    maxValueField.setText(""+max);    
  }

  public void setRange()
  {
    try
      {
	max = Float.parseFloat(maxValueField.getText());
	min = Float.parseFloat(minValueField.getText());
      }
    catch (NumberFormatException e1)
      {
	setVisible(false);
	return;
      }
    ((FtsVectorDisplayObject)target.getFtsObject()).setBounds(min, max);
  }

  public void actionPerformed( ActionEvent e)
  {
    if((e.getSource() == maxValueField)||(e.getSource() == minValueField)) 
      setRange();
  }
}











