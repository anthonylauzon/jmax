
package ircam.jmax.toolkit;

import java.awt.*;
import java.awt.event.*;
import com.sun.java.swing.*;

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
  static private final int WIDGET_HEIGHT = 30;

}

