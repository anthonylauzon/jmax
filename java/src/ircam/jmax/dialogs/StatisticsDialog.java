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
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * The "system statistics" dialog.
 */

public class StatisticsDialog extends Dialog implements ActionListener, KeyListener{
  Frame parent;
  Button okButton;
  
  public StatisticsDialog( Frame dw) {
    super(dw, "System statistics", false);
    parent = (Frame)dw;
    
    // Do a gc before giving statistics (added by mdc).
    
    System.gc();
    
    //Create middle section.
    Panel p1 = new Panel();
    p1.setLayout(new GridLayout(0, 1));

    Label label1 = new Label("jMax "+  MaxApplication.getProperty("jmaxVersion"));

    p1.add(label1);
    Label label2 = new Label("running on " + (String)(System.getProperties().getProperty("os.name")));
    p1.add(label2);
    Label label3 = new Label("host architecture:"+(String)(System.getProperties().getProperty("os.arch")));
    p1.add(label3);
    Label label4 = new Label("Java version" + (String)(System.getProperties().getProperty("java.version")));
    p1.add(label4);
    Label label8 = new Label("Total memory " + Runtime.getRuntime().totalMemory());
    p1.add(label8);
    Label label9 = new Label("Used memory " + (Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory()));
    p1.add(label9);

    add("Center", p1);
    
    //Create bottom row.
    Panel p2 = new Panel();
    p2.setLayout(new FlowLayout(FlowLayout.RIGHT));
    Button okButton = new Button("OK");
    okButton.addActionListener(this);
    p2.add(okButton);
    add("South", p2);

    addKeyListener(this);

    //Initialize this dialog to its preferred size.
    pack();  

    setLocation(100, 100);
    setVisible(true);
  }

  public void actionPerformed(ActionEvent e){    
    setVisible(false);
    dispose();      
  }
  
  //////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////keyListener--inizio
  public void keyTyped(KeyEvent e)
  {
    if (e.getKeyChar() == '\r')
      {
	setVisible(false);
	dispose();
      }
  }

  public void keyReleased(KeyEvent e){}
  
  public void keyPressed(KeyEvent e){}

  /////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////keyListener--fine
}










