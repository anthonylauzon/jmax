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

import ircam.jmax.MaxApplication;

import javax.swing.*;
import java.awt.*;
import java.io.File;
import java.awt.event.*;

/**
 * A graphic widget containing two (mini) buttons to control up and
 * down increments. The size of the widget allows to accomodate it 
 *in a statusbar */

public class IncrementController extends JPanel {
  
  /**
   * Constructor. The listener will be called when the user press
   * the up and down buttons */
  public IncrementController(IncrementListener l)
  {
    super();

    itsIncrementListener = l;

    String fs = File.separator;
    String path = MaxApplication.getProperty("root")+fs+"images"+fs;

    JButton up = new JButton(new ImageIcon(path+"little_up_arrow.gif"));
    JButton down = new JButton(new ImageIcon(path+"little_down_arrow.gif"));
    Insets i = new Insets(0,0,0,0);

    up.setMargin(i);
    down.setMargin(i);

    setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

    add(up);
    add(down);

    up.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e)
	{
	  itsIncrementListener.increment();
	}
    });

    down.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e)
	{
	  itsIncrementListener.decrement();
	}
    });

  }

  public Dimension getPreferredSize()
  {
    return new Dimension(15, 32);
  }

  public Dimension getMinimumSize()
  {
    return getPreferredSize();
  }

  //--- Fields 
  IncrementListener itsIncrementListener;
}


