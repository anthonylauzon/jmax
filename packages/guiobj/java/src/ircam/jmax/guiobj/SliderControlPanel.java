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

import ircam.jmax.editors.patcher.objects.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class SliderControlPanel extends JPanel implements ActionListener, ObjectControlPanel
{
  GraphicObject target = null;
  JTextField maxValueField, minValueField;
  JRadioButton horizontalItem, verticalItem; 

  public SliderControlPanel()
  {
    super();
    setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

    JLabel rangeLabel = new JLabel("Range", JLabel.CENTER);
    rangeLabel.setForeground(Color.black);

    Box labelRangeBox = new Box(BoxLayout.X_AXIS);
    labelRangeBox.add(Box.createRigidArea(new Dimension(20, 0)));    
    labelRangeBox.add(rangeLabel);    
    labelRangeBox.add(Box.createHorizontalGlue());    

    add(labelRangeBox);    

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

    add(minPanel);

    add(new JSeparator());

    JLabel orientLabel = new JLabel("Orientation", JLabel.CENTER);
    orientLabel.setForeground(Color.black);

    Box labelOrientBox = new Box(BoxLayout.X_AXIS);
    labelOrientBox.add(Box.createRigidArea(new Dimension(20, 0)));    
    labelOrientBox.add(orientLabel);    
    labelOrientBox.add(Box.createHorizontalGlue());    

    add(labelOrientBox);    

    ButtonGroup orientationButtonGroup = new ButtonGroup();
    JPanel orientationPanel = new JPanel();
    orientationPanel.setLayout(new BoxLayout(orientationPanel, BoxLayout.X_AXIS));
     
    horizontalItem = new JRadioButton("horizontal");
    horizontalItem.addItemListener(new ItemListener(){
	    public void itemStateChanged(ItemEvent e)
	    {
		if((e.getStateChange() == ItemEvent.SELECTED)&&(((Slider)target).getOrientation() != Slider.HORIZONTAL_OR))
		{
		  ((Slider)target).setOrientation(Slider.HORIZONTAL_OR);
		  ((Slider)target).updateDimension();
		}
	    }
	});
    orientationButtonGroup.add(horizontalItem);
    verticalItem = new JRadioButton("vertical");
    verticalItem.addItemListener(new ItemListener(){
	    public void itemStateChanged(ItemEvent e)
	    {
		if((e.getStateChange() == ItemEvent.SELECTED)&&(((Slider)target).getOrientation() != Slider.VERTICAL_OR))
		{
		  ((Slider)target).setOrientation(Slider. VERTICAL_OR);
		  ((Slider)target).updateDimension();
		}
	    }
	});
    orientationButtonGroup.add(verticalItem);
    
    orientationPanel.add(horizontalItem);
    orientationPanel.add(Box.createHorizontalGlue());    
    orientationPanel.add(verticalItem);
    orientationPanel.add(Box.createHorizontalGlue());    

    add(orientationPanel);

    validate();
  }

  public void update(GraphicObject obj)
  {
    target = obj;
    int min = ((Slider)obj).getMinValue();
    minValueField.setText(""+min);    
    int max = ((Slider)obj).getMaxValue();
    maxValueField.setText(""+max);    
    
    if(((Slider)obj).getOrientation() == Slider.HORIZONTAL_OR)
	horizontalItem.setSelected(true);
    else
	verticalItem.setSelected(true);
  }

  public void actionPerformed( ActionEvent e)
  {
      int max, min;
      if((e.getSource() == maxValueField)||(e.getSource() == minValueField)) 
	  {
	      try
		  {
		      max = Integer.parseInt(maxValueField.getText());
		      min = Integer.parseInt(minValueField.getText());
		  }
	      catch (NumberFormatException e1)
		  {
		      setVisible(false);
		      return;
		  }
	      ((Slider)target).setRange(max, min);
	  }
  }
}











