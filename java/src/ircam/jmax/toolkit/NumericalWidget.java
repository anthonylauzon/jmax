
package ircam.jmax.toolkit;

import com.sun.java.swing.*;
import java.awt.*;
import java.awt.event.*;

/**
 * An utility class to represent numerical fields in a status bar.*/
public class NumericalWidget extends Widget {
  
  /** Constructor. It builds a NuericalWidget with enough space for
   * n digits, in the given editing flavour (for now, only 
   * EDITABLE_FIELD supported */
  public NumericalWidget(String name, int n, int flavour)
  {
    super(name);

    itsTextField = new JTextField("");
    itsTextField.setSize(n*getFont().getSize(), NUMERICAL_WIDGET_HEIGHT);
    addController(itsTextField);
  }

  public void addActionListener(ActionListener l)
  {
    itsTextField.addActionListener(l);
  }

  public void setValue(int theValue)
  {
    itsTextField.setText(""+theValue);
  }

  public void setValue(String theValue)
  {
    itsTextField.setText(theValue);
	
  }

  public String getValue()
  {
    return itsTextField.getText();
  }

  public Component getCustomComponent()
  {
    return itsTextField;
  }

  //-- Fields
  static private final int NUMERICAL_WIDGET_HEIGHT = 18;
  static public final int EDITABLE_FIELD = 0;

  JTextField itsTextField;
}










