//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.widgets;

import java.awt.*;
import java.awt.event.*;
import java.beans.*;

import javax.swing.*;

public class ColorPropertyEditor extends PropertyEditorSupport {

  private JPanel panel;
  private JPanel colorField;
  private Color color;

  static
  {
    String[] path = PropertyEditorManager.getEditorSearchPath();
    String[] newPath = new String[ path.length + 1];

    newPath[0] = "ircam.jmax.widgets";
    System.arraycopy( path, 0, newPath, 1, path.length);

    PropertyEditorManager.setEditorSearchPath( newPath);

    for ( int i = 0; i < newPath.length; i++)
      System.err.println( newPath[i]);

    PropertyEditorManager.registerEditor( new java.awt.Color(10).getClass(),
					  null);
    PropertyEditorManager.registerEditor( new java.awt.Color(10).getClass(),
					  new ircam.jmax.widgets.ColorPropertyEditor().getClass());
  }

  public ColorPropertyEditor()
  {
    color = Color.white;

    panel = new JPanel();
    panel.setLayout( new BoxLayout( panel, BoxLayout.X_AXIS));

    colorField = new JPanel() {
      public Dimension getPreferredSize()
	{
	  return getMinimumSize();
	}
      public Dimension getMinimumSize()
	{
	  return new Dimension( 15, 15);
	}
    };
    colorField.setBackground( color);
    panel.add( colorField);

    JButton button = new JButton( "Choose");
    button.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent e)
	{
	  color = JColorChooser.showDialog( panel, "Color Chooser", color);
	}
    });
    panel.add( button);
  }

  public void setValue( Object value)
  {
    color = new Color( ((Color)value).getRGB());

    colorField.setBackground( color);
    colorField.repaint();
  }

  public Object getValue()
  {
    return color;
  }

  public boolean isPaintable()
  {
    return false;
  }

  public void paintValue( Graphics g, Rectangle r)
  {
  }

  public String getAsText()
  {
    return "red";
  }

  public void setAsText(String text) throws IllegalArgumentException
  {
  }

  public String[] getTags()
  {
    return null;
  }

  public Component getCustomEditor()
  {
    return panel;
  }

  public boolean supportsCustomEditor()
  {
    return true;
  }
}
