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

package ircam.jmax.dialogs;

import java.awt.*;
import java.awt.event.*;

import ircam.jmax.*;


/**
 * Provide an error message shown in a dialog; use by the static
 * function "show"
 */

public class ErrorDialog extends Dialog implements ActionListener{
  Frame parent;
  String itsError;
  Button okButton;

  public ErrorDialog(Frame dw, String theError) {
    super(dw , "Error", true);
    parent = dw;
    itsError = theError;

    //Create middle section.
    Panel p1 = new Panel();
    Label label = new Label(itsError);
    p1.add(label);
    add("Center", p1);

    //Create bottom row.
    Panel p2 = new Panel();
    p2.setLayout(new FlowLayout(FlowLayout.RIGHT));
    Button okButton = new Button("OK");
    okButton.addActionListener(this);
    p2.add(okButton);
    add("South", p2);

    //Initialize this dialog to its preferred size.
    pack();

    // locate and Show.

    setLocation(100, 100);
    show();  
  }

  public void actionPerformed(ActionEvent e){    
    setVisible(false);
    dispose();
  }
}





