 //
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.toolkit;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class Widget extends JPanel {

  /**
   * create a named widget without controllers (only the name) */
  public Widget(String theName)
  {
    super();
    setLayout(null);

    itsLabel = new JLabel(theName);
    
    itsLabel.setSize(WIDGET_LABEL_WIDTH, WIDGET_HEIGHT);

    itsLabel.setLocation(0,0);
    itsLabel.setVerticalAlignment(JLabel.CENTER);

    add(itsLabel);
    itsLabel.setSize(WIDGET_LABEL_WIDTH, WIDGET_HEIGHT);
  }

  /**
   * create a named widget with the given component as controller */
  public Widget(String theName, Component theController)
  {
    super();
    setLayout(null);

    itsLabel = new JLabel(theName);
    
    itsLabel.setSize(WIDGET_LABEL_WIDTH, WIDGET_HEIGHT);

    itsLabel.setLocation(0,0);
    itsLabel.setVerticalAlignment(JLabel.CENTER);
    add(itsLabel);
    addController(theController);
  }

  public String getName()
  {
    return itsLabel.getText();
  }

  public void setName(String theName)
  {
    itsLabel.setText(theName);
  }

  public Component getController()
  {
    return itsController;
  }

  public void addController(Component theController)
  {
    if (itsController != null) return; //hey! ONE controller per widget!
    
    itsController = theController;
    if (itsLabel != null) 
      {
	itsController.setLocation(WIDGET_LABEL_WIDTH + 1, 0);
      }
    else 
      {
	itsController.setLocation(0,0);
      }

    add(itsController);
    setSize(WIDGET_LABEL_WIDTH+itsController.getSize().width, WIDGET_HEIGHT);
  }

  public void addActionListener(ActionListener l) {}

  public void setValue(String value) {}
  public void setValue(int value) {}
  public void setValue(boolean value) {}

  public String getValue() {return "";}

  //-- Fields

  Component itsController;
  JLabel itsLabel;

  static private final int WIDGET_LABEL_WIDTH = 40;
  static public final int WIDGET_HEIGHT = 30;

}

