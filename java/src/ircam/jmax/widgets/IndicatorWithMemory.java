package ircam.jmax.widgets;

import com.sun.java.swing.*;
import com.sun.java.swing.text.*;
import com.sun.java.swing.border.*;

import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

public class IndicatorWithMemory extends JPanel {
  boolean value = false;

  public IndicatorWithMemory(String name)
  {
    Border border = new TitledBorder(null, name, TitledBorder.LEFT, TitledBorder.TOP);

    setBorder( border);
    setLayout( new BoxLayout( this, BoxLayout.X_AXIS));

    JLabel label;

    label = new JLabel( name + ": ");
    indicatorLed = new Led();
    label.setLabelFor( indicatorLed);
    indicatorLed.setAlignmentX( CENTER_ALIGNMENT);
    add( label);
    add( indicatorLed);

    add( Box.createVerticalStrut( 2));

    label = new JLabel( "Memory : ");
    memoryLed = new Led();
    memoryLed.setAlignmentX( CENTER_ALIGNMENT);
    label.setLabelFor( memoryLed);
    add( label);
    add( memoryLed);

    add( Box.createVerticalStrut( 2));

    JButton button = new JButton( "Reset");
    button.setFocusPainted( false);
    add( button);
    button.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent event)
	{
	  reset();
	}
    });
  }

  public Dimension getMinimumSize()
  {
    return new Dimension( 100, 40);
  }

  public Dimension getPreferredSize()
  {
    return getMinimumSize();
  }

  public void setValue( boolean value)
  {
    this.value = value;

    indicatorLed.setState( value ? Led.ON : Led.OFF);

    if (value)
      memoryLed.setState(Led.ON);
  }

  public void reset()
  {
    if (! value)
      memoryLed.setState( Led.OFF);
  }

  protected Led indicatorLed;
  protected Led memoryLed;
}

