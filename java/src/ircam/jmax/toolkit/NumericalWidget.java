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

package ircam.jmax.toolkit;

// import javax.swing.*;
import javax.swing.JTextField;

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










