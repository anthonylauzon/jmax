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

//import javax.swing.*;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JSeparator;
import javax.swing.JTextField;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;
import ircam.jmax.widgets.*;

import ircam.jmax.editors.patcher.objects.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class SliderControlPanel extends JPanel implements ActionListener, ObjectControlPanel
{
  GraphicObject target = null;
  JTextField maxValueField, minValueField, nameField;
  JRadioButton horizontalItem, verticalItem; 
  JLabel maxLabel, minLabel;
  JPanel minPanel, maxPanel;
  JCheckBox persistentCB = null;
  int min, max;
  String name = null;

  public SliderControlPanel( GraphicObject obj)
  {
    super();
    setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
    
    target = obj;

    JLabel rangeLabel = new JLabel("Range", JLabel.CENTER);
    rangeLabel.setForeground(Color.black);

    Box labelRangeBox = new Box(BoxLayout.X_AXIS);
    labelRangeBox.add(Box.createRigidArea(new Dimension(20, 0)));    
    labelRangeBox.add(rangeLabel);    
    labelRangeBox.add(Box.createHorizontalGlue());    

    add(labelRangeBox);    

    maxLabel = new JLabel("top", JLabel.CENTER);
    maxValueField = new JTextField();
    
    maxValueField.setPreferredSize(new Dimension(100, 20));
    maxValueField.setMaximumSize(new Dimension(100, 20));
    maxValueField.addActionListener(this);
    maxValueField.requestDefaultFocus();

    maxPanel = new JPanel();
    maxPanel.setPreferredSize(new Dimension(170, 20));
    maxPanel.setLayout(new BoxLayout(maxPanel, BoxLayout.X_AXIS));    
    maxPanel.add(Box.createRigidArea(new Dimension(5, 0)));    
    maxPanel.add(maxLabel);
    maxPanel.add(Box.createHorizontalGlue());    
    maxPanel.add( maxValueField);
    maxPanel.add(new IncrementController(new IncrementListener(){
	public void increment( boolean shiftPressed)
	{
	  int incr;
	  if( shiftPressed)
	    incr = 10;
	  else
	    incr = 1;
	  
	  max+=incr;
	  maxValueField.setText(""+max);    
	}
	public void decrement( boolean shiftPressed)
	{
	  int incr;
	  if( shiftPressed)
	    incr = 10;
	  else
	    incr = 1;

	  max-=incr;
	  maxValueField.setText(""+max);    
	}
      }));
    maxPanel.validate();    

    add(maxPanel);

    minLabel = new JLabel("bottom", JLabel.CENTER);
    minValueField = new JTextField();
    minValueField.setPreferredSize(new Dimension(100, 20));
    minValueField.setMaximumSize(new Dimension(100, 20));
    minValueField.addActionListener(this);

    minPanel = new JPanel();
    minPanel.setPreferredSize(new Dimension(170, 20));
    minPanel.setLayout(new BoxLayout(minPanel, BoxLayout.X_AXIS));
    minPanel.add(Box.createRigidArea(new Dimension(5, 0)));    
    minPanel.add(minLabel);
    minPanel.add(Box.createHorizontalGlue());    
    minPanel.add( minValueField);
    minPanel.add(new IncrementController(new IncrementListener(){
	public void increment( boolean shiftPressed)
	{
	  int incr;
	  if( shiftPressed)
	    incr = 10;
	  else
	    incr = 1;
	  
	  min+=incr;
	  minValueField.setText(""+min);    
	}
	public void decrement( boolean shiftPressed)
	{
	  int incr;
	  if( shiftPressed)
	    incr = 10;
	  else
	    incr = 1;

	  min-=incr;
	  minValueField.setText(""+min);    
	}
      }));    
    minPanel.validate();

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
	    changeOrientation( Slider.HORIZONTAL_OR);
	}
      });
    orientationButtonGroup.add(horizontalItem);
    verticalItem = new JRadioButton("vertical");
    verticalItem.addItemListener(new ItemListener(){
	public void itemStateChanged(ItemEvent e)
	{
	  if((e.getStateChange() == ItemEvent.SELECTED)&&(((Slider)target).getOrientation() != Slider.VERTICAL_OR))
	    changeOrientation( Slider.VERTICAL_OR);
	}
      });
    orientationButtonGroup.add(verticalItem);
    
    orientationPanel.add(horizontalItem);
    orientationPanel.add(Box.createHorizontalGlue());    
    orientationPanel.add(verticalItem);
    orientationPanel.add(Box.createHorizontalGlue());    

    add(orientationPanel);

    add( new JSeparator());

    /* persistence handling */
    persistentCB = new JCheckBox("Persistence");
    persistentCB.setAlignmentX( Component.RIGHT_ALIGNMENT);
    persistentCB.addActionListener( new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  int persist = ((JCheckBox)e.getSource()).isSelected() ? 1 : 0;
	  target.getFtsObject().requestSetPersistent( persist);
	}
      });    
    add( persistentCB);

    add(new JSeparator());

    /* name handling */
    JLabel nameLabel = new JLabel("Name", JLabel.RIGHT);
    nameLabel.setForeground(Color.black);

    nameField = new JTextField();
    nameField.setPreferredSize(new Dimension(150, 20));
    nameField.addActionListener(this);

    JPanel namePanel = new JPanel();
    namePanel.setLayout( new BoxLayout( namePanel, BoxLayout.X_AXIS));    
    namePanel.add(Box.createRigidArea(new Dimension(5, 0)));  
    namePanel.add(nameLabel);    
    namePanel.add(Box.createRigidArea(new Dimension(5, 0)));  
    namePanel.add( nameField);

    add(namePanel);

    update( obj);
    validate();
  }

  void changeOrientation(int or)
  {
    ((Slider)target).setOrientation( or);
    ((Slider)target).updateDimension();
    if( or == Slider.VERTICAL_OR)
      {
	maxLabel.setText("top");
	minLabel.setText("bottom");
      }
    else
      {
	maxLabel.setText("right");
	minLabel.setText("left");
      }
  }

  public void update(GraphicObject obj)
  {
    target = obj;
    min = ((Slider)obj).getMinValue();
    minValueField.setText(""+min);    
    max = ((Slider)obj).getMaxValue();
    maxValueField.setText(""+max);    

    if(((Slider)obj).getOrientation() == Slider.HORIZONTAL_OR)
      {
	horizontalItem.setSelected(true);
	maxLabel.setText("right");
	minLabel.setText("left");
      }    
    else
      {
	verticalItem.setSelected(true);
	maxLabel.setText("top");
	minLabel.setText("bottom");		  
      }
    
    persistentCB.setSelected( target.getFtsObject().isPersistent() == 1);
    
    name = target.getFtsObject().getVariableName();
    nameField.setText( name);    
  }

  public void done()
  {
    setRange();
    setName();
  }

  public void setRange()
  {
    try
      {
	max = Integer.parseInt(maxValueField.getText());
	min = Integer.parseInt(minValueField.getText());
	if(max<min)
	  {
	    int temp = max;
	    max=min;min=temp;
	  }
      }
    catch (NumberFormatException e1)
      {
	return;
      }
    ((Slider)target).setRange(max, min);
  }

  public void actionPerformed( ActionEvent e)
  {
    if((e.getSource() == maxValueField)||(e.getSource() == minValueField)) 
      {
	setRange();
	setName();
      }  
  }

  public void setName()
  {
    if( nameField != null)
      {
	String text = nameField.getText().trim();
	
	if( text != null)
	  if( ((this.name == null) && !text.equals("")) || ((this.name != null) && !text.equals( this.name)))
	    {
	      target.getFtsObject().requestSetName( text);
	      name = text;
	    }
      }
  }
}











