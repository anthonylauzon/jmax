package ircam.jmax.widgets;

import com.sun.java.swing.*;
import com.sun.java.swing.text.*;
import com.sun.java.swing.border.*;

import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

public class DacSlipPanel extends JPanel {

  public DacSlipPanel()
  {
    Border border = new TitledBorder(null, "Dac slip", TitledBorder.LEFT, TitledBorder.TOP);

    setBorder( border);
    setLayout( new BoxLayout( this, BoxLayout.X_AXIS));

    JLabel label;

    label = new JLabel( "Dac slip: ");
    dacSlipLed = new Led();
    label.setLabelFor( dacSlipLed);
    dacSlipLed.setAlignmentX( CENTER_ALIGNMENT);
    add( label);
    add( dacSlipLed);

    add( Box.createVerticalStrut( 2));

    label = new JLabel( "Indicator: ");
    indicatorLed = new Led();
    indicatorLed.setAlignmentX( CENTER_ALIGNMENT);
    label.setLabelFor( indicatorLed);
    add( label);
    add( indicatorLed);

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

  public void setDacSlip( boolean dacSlip)
  {
    dacSlipLed.setState( dacSlip ? Led.ON : Led.OFF);

    if (dacSlip)
      {
	indicatorLed.setState( dacSlip ? Led.ON : Led.OFF);
      }
  }

  public void reset()
  {
    dacSlipLed.setState( Led.OFF);
    dacSlipLed.setState( Led.OFF);
  }

  protected Led dacSlipLed;
  protected Led indicatorLed;
}

