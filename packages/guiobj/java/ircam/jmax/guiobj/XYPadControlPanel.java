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

public class XYPadControlPanel extends JPanel implements ActionListener, ObjectControlPanel
{
  GraphicObject target = null;
  JTextField xMaxValueField, xMinValueField, yMaxValueField, yMinValueField;
  JLabel xMaxLabel, xMinLabel, yMaxLabel, yMinLabel;
  JPanel xMinPanel, xMaxPanel, yMinPanel, yMaxPanel;
  int xMin, xMax, yMin, yMax;

  public XYPadControlPanel()
  {
    super();
    setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

    JLabel xRangeLabel = new JLabel("X Range", JLabel.CENTER);
    xRangeLabel.setForeground(Color.black);

    Box xLabelRangeBox = new Box(BoxLayout.X_AXIS);
    xLabelRangeBox.add(Box.createRigidArea(new Dimension(20, 0)));    
    xLabelRangeBox.add(xRangeLabel);    
    xLabelRangeBox.add(Box.createHorizontalGlue());    

    add(xLabelRangeBox);    

    xMaxLabel = new JLabel("top", JLabel.CENTER);
    xMaxValueField = new JTextField();
    xMaxValueField.setPreferredSize(new Dimension(100, 20));
    xMaxValueField.setMaximumSize(new Dimension(100, 20));
    xMaxValueField.addActionListener(this);
    xMaxValueField.requestDefaultFocus();
    
    xMaxPanel = new JPanel();
    xMaxPanel.setPreferredSize(new Dimension(170, 20));
    xMaxPanel.setLayout(new BoxLayout(xMaxPanel, BoxLayout.X_AXIS));    
    xMaxPanel.add(Box.createRigidArea(new Dimension(5, 0)));    
    xMaxPanel.add(xMaxLabel);
    xMaxPanel.add(Box.createHorizontalGlue());    
    xMaxPanel.add( xMaxValueField);
    xMaxPanel.add(new IncrementController(new IncrementListener(){
	public void increment()
	{
	  xMax++;
	  xMaxValueField.setText(""+xMax);    
	}
	public void decrement()
	{
	  xMax--;
	  xMaxValueField.setText(""+xMax);    
	}
      }));
    xMaxPanel.validate();    

    add(xMaxPanel);

    xMinLabel = new JLabel("bottom", JLabel.CENTER);
    xMinValueField = new JTextField();
    xMinValueField.setPreferredSize(new Dimension(100, 20));
    xMinValueField.setMaximumSize(new Dimension(100, 20));
    xMinValueField.addActionListener(this);
    
    xMinPanel = new JPanel();
    xMinPanel.setPreferredSize(new Dimension(170, 20));
    xMinPanel.setLayout(new BoxLayout(xMinPanel, BoxLayout.X_AXIS));
    xMinPanel.add(Box.createRigidArea(new Dimension(5, 0)));    
    xMinPanel.add(xMinLabel);
    xMinPanel.add(Box.createHorizontalGlue());    
    xMinPanel.add( xMinValueField);
    xMinPanel.add(new IncrementController(new IncrementListener(){
	public void increment()
	{
	  xMin++;
	  xMinValueField.setText(""+xMin);    
	}
	public void decrement()
	{
	  xMin--;
	  xMinValueField.setText(""+xMin);    
	}
      }));    
    xMinPanel.validate();

    add(xMinPanel);

    add(new JSeparator());

    JLabel yRangeLabel = new JLabel("Y Range", JLabel.CENTER);
    yRangeLabel.setForeground(Color.black);

    Box yLabelRangeBox = new Box(BoxLayout.X_AXIS);
    yLabelRangeBox.add(Box.createRigidArea(new Dimension(20, 0)));    
    yLabelRangeBox.add(yRangeLabel);    
    yLabelRangeBox.add(Box.createHorizontalGlue());    

    add(yLabelRangeBox);    

    yMaxLabel = new JLabel("top", JLabel.CENTER);
    yMaxValueField = new JTextField();
    yMaxValueField.setPreferredSize(new Dimension(100, 20));
    yMaxValueField.setMaximumSize(new Dimension(100, 20));
    yMaxValueField.addActionListener(this);
    yMaxValueField.requestDefaultFocus();
    
    yMaxPanel = new JPanel();
    yMaxPanel.setPreferredSize(new Dimension(170, 20));
    yMaxPanel.setLayout(new BoxLayout(yMaxPanel, BoxLayout.X_AXIS));    
    yMaxPanel.add(Box.createRigidArea(new Dimension(5, 0)));    
    yMaxPanel.add(yMaxLabel);
    yMaxPanel.add(Box.createHorizontalGlue());    
    yMaxPanel.add(yMaxValueField);
    yMaxPanel.add(new IncrementController(new IncrementListener(){
	public void increment()
	{
	  yMax++;
	  yMaxValueField.setText(""+yMax);    
	}
	public void decrement()
	{
	  yMax--;
	  yMaxValueField.setText(""+yMax);    
	}
      }));
    yMaxPanel.validate();    

    add(yMaxPanel);

    yMinLabel = new JLabel("bottom", JLabel.CENTER);
    yMinValueField = new JTextField();
    yMinValueField.setPreferredSize(new Dimension(100, 20));
    yMinValueField.setMaximumSize(new Dimension(100, 20));
    yMinValueField.addActionListener(this);
    
    yMinPanel = new JPanel();
    yMinPanel.setPreferredSize(new Dimension(170, 20));
    yMinPanel.setLayout(new BoxLayout(yMinPanel, BoxLayout.X_AXIS));
    yMinPanel.add(Box.createRigidArea(new Dimension(5, 0)));    
    yMinPanel.add(yMinLabel);
    yMinPanel.add(Box.createHorizontalGlue());    
    yMinPanel.add( yMinValueField);
    yMinPanel.add(new IncrementController(new IncrementListener(){
	public void increment()
	{
	  yMin++;
	  yMinValueField.setText(""+yMin);    
	}
	public void decrement()
	{
	  yMin--;
	  yMinValueField.setText(""+yMin);    
	}
      }));    
    yMinPanel.validate();

    add(yMinPanel);

    validate();
  }


  public void update(GraphicObject obj)
  {
    target = obj;
    xMin = ((XYPad)obj).getXMinValue();
    xMinValueField.setText(""+xMin);    
    xMax = ((XYPad)obj).getXMaxValue();
    xMaxValueField.setText(""+xMax);    
    xMaxLabel.setText("top");
    xMinLabel.setText("bottom");		  
   
    yMin = ((XYPad)obj).getYMinValue();
    yMinValueField.setText(""+yMin);    
    yMax = ((XYPad)obj).getYMaxValue();
    yMaxValueField.setText(""+yMax);    
    yMaxLabel.setText("top");
    yMinLabel.setText("bottom");	
  }

  public void done()
  {
    setRange();
  }

  public void setRange()
  {
    try
      {
	xMax = Integer.parseInt(xMaxValueField.getText());
	xMin = Integer.parseInt(xMinValueField.getText());
	yMax = Integer.parseInt(yMaxValueField.getText());
	yMin = Integer.parseInt(yMinValueField.getText());
	if(xMax<xMin)
	  {
	    int temp = xMax;
	    xMax=xMin;xMin=temp;
	  }
	if(yMax<yMin)
	  {
	    int temp = yMax;
	    yMax=yMin;yMin=temp;
	  }
      }
    catch (NumberFormatException e1)
      {
	return;
      }
    ((XYPad)target).setXRange(xMax, xMin);
    ((XYPad)target).setYRange(yMax, yMin);
  }

  public void actionPerformed( ActionEvent e)
  {
    if((e.getSource() == xMaxValueField)||(e.getSource() == xMinValueField)|| 
       (e.getSource() == yMaxValueField)||(e.getSource() == yMinValueField))
      setRange();
  }
}











