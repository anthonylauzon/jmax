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
package ircam.jmax.toolkit;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.*;
import java.util.*;

/**
 * the class implementing a generic status bar .
 * It implements the StatusBar interface
 */
public class InfoPanel extends JPanel implements StatusBar{
  
  /** constructor */
  public InfoPanel() {

    setLayout(null);

    setBackground(Color.lightGray);
    setBorder(new BevelBorder(BevelBorder.RAISED));
    
    itsHeaderFont = new Font(getFont().getName(), Font.BOLD, getFont().getSize());
    itsHeaderLabel = new JLabel();
    itsHeaderLabel.setFont(itsHeaderFont);

    itsHeaderLabel.setSize(headerPreferredSize.width, headerPreferredSize.height);
    addWidget(itsHeaderLabel);

    itsLabel = new JLabel();
    itsLabel.setBackground(Color.white);

    itsLabel.setSize(labelPreferredSize.width, labelPreferredSize.height);
    addWidget(itsLabel);

  }
  
  /** Status bar interface */
  public void post(StatusBarClient theClient, String message) 
  {
    if (theClient.getIcon() != null)    
      itsHeaderLabel.setIcon(theClient.getIcon());
    itsHeaderLabel.setText(theClient.getName()+":");
    itsLabel.setText(message);
  }

  /**
   * used to add controllers to the status bar.
   * The size of the controller will be adjusted to fit into the standard height */
  public void addWidget(Component theWidget)
  {
    if (theWidget.getSize().height > INFO_HEIGHT-8)
      theWidget.setSize(theWidget.getSize().width, INFO_HEIGHT-8);

    theWidget.setLocation(availablePosition, (INFO_HEIGHT-theWidget.getSize().height)/2);

    add(theWidget);
    widgets.addElement(theWidget);

    availablePosition+=theWidget.getSize().width+2;
  }

  /**
   * add a controllers to the status bar in the given position.
   * The height of the controller will be adjusted to fit into the standard 
   * height */
  public void addWidgetAt(Component theWidget, int position)
  {
    if (position >= widgets.size() || position < 0) 
      {
	addWidget(theWidget);
	return;
      }

    Component c = (Component) widgets.elementAt(position);
    int xposition = c.getLocation().x;

    theWidget.setLocation(xposition, 4);

    if (theWidget.getSize().height > INFO_HEIGHT-8)
      theWidget.setSize(theWidget.getSize().width, INFO_HEIGHT-8);

    add(theWidget);

    xposition += theWidget.getSize().width+2;

    for (int i = position; i<widgets.size(); i++)
      {
	c = (Component) widgets.elementAt(i);
	c.setLocation(xposition, c.getLocation().y);
	xposition += c.getSize().width+2;
      }

    widgets.insertElementAt(theWidget, position);
    availablePosition = xposition;
  }
  
  /**
   * remove a controller in the status bar */
  //this implementation is stupid. Must use a better Layout instead
  public void removeWidget(Component theWidget)
  {
    int widgetIndex = widgets.indexOf(theWidget);
    int xposition = ((Component) widgets.elementAt(widgetIndex-1)).getSize().width+2;

    Component c;
    for (int i = widgetIndex+1; i<widgets.size(); i++)
      {
	c = (Component) widgets.elementAt(i);
	c.setLocation(xposition, c.getLocation().y);
	xposition += c.getSize().width+2;
      }

    remove(theWidget);
    widgets.removeElement(theWidget);
  }

  public Dimension getPreferredSize()
  {
    return getMinimumSize();
  }

  public Dimension getMinimumSize() 
  {
    return infoSize;
  }

  //----- Fields
  public static final int INFO_WIDTH = 300;
  public static final int INFO_HEIGHT = 30;
  private static final int INITIAL_AVAILABLE_POSITION = 10;
  Dimension infoSize = new Dimension(INFO_WIDTH, INFO_HEIGHT);
  Font itsHeaderFont;
  JLabel itsLabel;

  JLabel itsHeaderLabel;
  Dimension headerPreferredSize = new Dimension(100, INFO_HEIGHT);
  Dimension labelPreferredSize = new Dimension(100, INFO_HEIGHT);

  Vector widgets = new Vector();  
  int availablePosition = INITIAL_AVAILABLE_POSITION;
}

