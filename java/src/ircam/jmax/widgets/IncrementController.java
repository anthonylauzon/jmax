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


