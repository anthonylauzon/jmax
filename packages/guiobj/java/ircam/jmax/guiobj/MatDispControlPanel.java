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
import javax.swing.JButton;
import javax.swing.JColorChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.JSeparator;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;
import ircam.jmax.widgets.*;

import ircam.jmax.editors.patcher.objects.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class MatDispControlPanel extends JPanel implements ActionListener, ObjectControlPanel
{
  GraphicObject target = null;
  JTextField maxValueField, minValueField, nZoomValueField, mZoomValueField;
  float min, max;
  int nZoom, mZoom;
  JButton minColorButton;
  JButton maxColorButton;

  public MatDispControlPanel()
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
    maxValueField.setPreferredSize(new Dimension(80, 20));
    maxValueField.setMaximumSize(new Dimension(80, 20));
    maxValueField.addActionListener(this);
    
    maxColorButton = new JButton();
    maxColorButton.setPreferredSize(new Dimension(20, 20));
    maxColorButton.setMaximumSize(new Dimension(20, 20));
    maxColorButton.setMinimumSize(new Dimension(20, 20));
    maxColorButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  FtsMatDisplayObject obj = (FtsMatDisplayObject)target.getFtsObject();
	  Color currColor = new Color(obj.getMaxColor());
	  Color newColor = JColorChooser.showDialog(null,"Choose Max Color", currColor);
	  
	  if(newColor != null){
	    obj.setMaxColor(newColor.getRGB());
	    maxColorButton.setBackground(newColor);
	    maxColorButton.repaint();
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
    maxPanel.add(new IncrementController(new IncrementListener(){
	public void increment( boolean shiftPressed)
	{	  
	  float incr;
	  if( shiftPressed)
	    incr = 1.0f;
	  else
	    incr = 0.1f;

	  max += incr;
	  maxValueField.setText(""+max);    
	}
	public void decrement( boolean shiftPressed)
	{
	  float incr;
	  if( shiftPressed)
	    incr = 1.0f;
	  else
	    incr = 0.1f;

	  max -= incr;
	  maxValueField.setText(""+max);    
	}
      }));
    maxPanel.add( maxColorButton);
    maxPanel.validate(); 

    add(maxPanel);

    JLabel minLabel = new JLabel("min", JLabel.CENTER);
    minValueField = new JTextField();
    minValueField.setPreferredSize(new Dimension(80, 20));
    minValueField.setMaximumSize(new Dimension(80, 20));
    minValueField.addActionListener(this);

    minColorButton = new JButton();
    minColorButton.setPreferredSize(new Dimension(20, 20));
    minColorButton.setMaximumSize(new Dimension(20, 20));
    minColorButton.setMinimumSize(new Dimension(20, 20));
    minColorButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	{
	  FtsMatDisplayObject obj = (FtsMatDisplayObject)target.getFtsObject();
	  Color currColor = new Color(obj.getMinColor());
	  Color newColor = JColorChooser.showDialog(null,"Choose Min Color", currColor);
	  
	  if(newColor != null){
	    obj.setMinColor(newColor.getRGB());
	    minColorButton.setBackground(newColor);
	    minColorButton.repaint();
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
    minPanel.add(new IncrementController(new IncrementListener(){
	public void increment( boolean shiftPressed)
	{
	  float incr;
	  if( shiftPressed)
	    incr = 1.0f;
	  else
	    incr = 0.1f;

	  min += incr;
	  minValueField.setText(""+min);    
	}
	public void decrement( boolean shiftPressed)
	{
	  float incr;
	  if( shiftPressed)
	    incr = 1.0f;
	  else
	    incr = 0.1f;

	  min -= incr;
	  minValueField.setText(""+min);    
	}
      }));    
    minPanel.add( minColorButton);
    minPanel.validate();

    add(minPanel);

    add( new JSeparator());

    JLabel zoomLabel = new JLabel("Zoom", JLabel.CENTER);
    zoomLabel.setForeground(Color.black);

    Box zoomLabelBox = new Box(BoxLayout.X_AXIS);
    zoomLabelBox.add(Box.createRigidArea(new Dimension(20, 0)));    
    zoomLabelBox.add(zoomLabel);    
    zoomLabelBox.add(Box.createHorizontalGlue());    

    add(zoomLabelBox);    

    JLabel xLabel = new JLabel("X", JLabel.CENTER);
    nZoomValueField = new JTextField();
    nZoomValueField.setPreferredSize(new Dimension(100, 20));
    nZoomValueField.setMaximumSize(new Dimension(100, 20));
    nZoomValueField.addActionListener(this);
    
    JPanel nZoomPanel = new JPanel();
    nZoomPanel.setPreferredSize(new Dimension(150, 20));
    nZoomPanel.setLayout(new BoxLayout(nZoomPanel, BoxLayout.X_AXIS));    
    nZoomPanel.add(Box.createRigidArea(new Dimension(5, 0)));    
    nZoomPanel.add(xLabel);
    nZoomPanel.add(Box.createHorizontalGlue());    
    nZoomPanel.add( nZoomValueField);
    nZoomPanel.add(new IncrementController(new IncrementListener(){
	public void increment( boolean shiftPressed)
	{
	  int incr;
	  if( shiftPressed)
	    incr = 10;
	  else
	    incr = 1;

	  nZoom+=incr;
	  nZoomValueField.setText(""+nZoom);    
	}
	public void decrement( boolean shiftPressed)
	{
	  int incr;
	  if( shiftPressed)
	    incr = 10;
	  else
	    incr = 1;

	  nZoom-=incr;
	  nZoomValueField.setText(""+nZoom);    
	}
      }));
    nZoomPanel.validate(); 

    add(nZoomPanel);

    JLabel yLabel = new JLabel("Y", JLabel.CENTER);
    mZoomValueField = new JTextField();
    mZoomValueField.setPreferredSize(new Dimension(100, 20));
    mZoomValueField.setMaximumSize(new Dimension(100, 20));
    mZoomValueField.addActionListener(this);
    
    JPanel mZoomPanel = new JPanel();
    mZoomPanel.setPreferredSize(new Dimension(150, 20));
    mZoomPanel.setLayout(new BoxLayout(mZoomPanel, BoxLayout.X_AXIS));
    mZoomPanel.add(Box.createRigidArea(new Dimension(5, 0)));    
    mZoomPanel.add(yLabel);
    mZoomPanel.add(Box.createHorizontalGlue());    
    mZoomPanel.add( mZoomValueField);
    mZoomPanel.add(new IncrementController(new IncrementListener(){
	public void increment( boolean shiftPressed)
	{
	  int incr;
	  if( shiftPressed)
	    incr = 10;
	  else
	    incr = 1;

	  mZoom+=incr;
	  mZoomValueField.setText(""+mZoom);    
	}
	public void decrement( boolean shiftPressed)
	{
	  int incr;
	  if( shiftPressed)
	    incr = 10;
	  else
	    incr = 1;

	  mZoom-=incr;
	  mZoomValueField.setText(""+mZoom);    
	}
      }));    
    mZoomPanel.validate();

    add(mZoomPanel);

    validate();
  }

  public void update(GraphicObject obj)
  {
    FtsMatDisplayObject ftsObj = (FtsMatDisplayObject)obj.getFtsObject();

    target = obj;

    min = ftsObj.getMinimum();
    minValueField.setText(""+min);    
    max = ftsObj.getMaximum();
    maxValueField.setText(""+max);    

    nZoom = ftsObj.getNZoom();
    nZoomValueField.setText(""+nZoom);    
    mZoom = ftsObj.getMZoom();
    mZoomValueField.setText(""+mZoom);

    minColorButton.setBackground(new Color(ftsObj.getMinColor()));
    maxColorButton.setBackground(new Color(ftsObj.getMaxColor()));
  }

  public void done()
  {
    setRange();
    setZoom();
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
	return;
      }
    ((FtsMatDisplayObject)target.getFtsObject()).setRange(min, max);
  }

 public void setZoom()
  {
    try
      {
	nZoom = Integer.parseInt(nZoomValueField.getText());
	mZoom = Integer.parseInt(mZoomValueField.getText());
      }
    catch (NumberFormatException e1)
      {
	return;
      }
    ((FtsMatDisplayObject)target.getFtsObject()).setZoom(mZoom, nZoom);
  }

  public void actionPerformed( ActionEvent e)
  {
    if((e.getSource() == maxValueField)||(e.getSource() == minValueField)) 
      setRange();

    if((e.getSource() == nZoomValueField)||(e.getSource() == mZoomValueField)) 
      setZoom();
  }
}











