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

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

/**
 * The "system statistics" dialog.
 */

public class StatisticsDialog extends JDialog implements ActionListener, KeyListener{
  Frame parent;
  JButton okButton;
  private FtsDspControl control;

  public StatisticsDialog( Frame dw) {

    super(dw, "System statistics", false);

    parent = (Frame)dw;
  
    control = MaxApplication.getFts().getDspController();
    
    // Do a gc before giving statistics (added by mdc).    
    System.gc();
    
    //Create middle section.
    JPanel p1 = new JPanel();
    p1.setLayout(new BoxLayout(p1, BoxLayout.Y_AXIS));

    JPanel p11 = new JPanel();
    p11.setLayout(new BoxLayout(p11, BoxLayout.X_AXIS));
    p11.setBorder(BorderFactory.createEtchedBorder());

    JPanel pNames = new JPanel();
    pNames.setLayout(new BoxLayout(pNames, BoxLayout.Y_AXIS));
    pNames.setPreferredSize(new Dimension(130, 90));
    
    JLabel name1 = new JLabel("jMax");
    pNames.add(name1);
    JLabel name2 = new JLabel("running on ");
    pNames.add(name2);
    JLabel name3 = new JLabel("host architecture");
    pNames.add(name3);
    JLabel name4 = new JLabel("Java version");
    pNames.add(name4);
    JLabel name5 = new JLabel("Total memory ");
    pNames.add(name5);
    JLabel name6 = new JLabel("Used memory ");
    pNames.add(name6);

    p11.add(pNames);

    p11.add(Box.createHorizontalGlue());

    JPanel pValues = new JPanel();
    pValues.setLayout(new BoxLayout(pValues, BoxLayout.Y_AXIS));
    pValues.setPreferredSize(new Dimension(130, 90));
    
    JLabel value1 = new JLabel(MaxApplication.getProperty("jmaxVersion"));
    pValues.add(value1);
    JLabel value2 = new JLabel((String)(System.getProperties().getProperty("os.name")));
    pValues.add(value2);
    JLabel value3 = new JLabel((String)(System.getProperties().getProperty("os.arch")));
    pValues.add(value3);
    JLabel value4 = new JLabel((String)(System.getProperties().getProperty("java.version")));
    pValues.add(value4);
    JLabel value5 = new JLabel("" + Runtime.getRuntime().totalMemory());
    pValues.add(value5);
    JLabel value6 = new JLabel("" + (Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory()));
    pValues.add(value6);

    p11.add(pValues);

    p1.add(p11);

    JPanel p12 = new JPanel();
    p12.setLayout(new BoxLayout(p12, BoxLayout.X_AXIS));
    p12.setBorder(BorderFactory.createEtchedBorder());

    JPanel pNames2 = new JPanel();
    pNames2.setLayout(new BoxLayout(pNames2, BoxLayout.Y_AXIS));
    pNames2.setPreferredSize(new Dimension(130, 30));
    
    JLabel name7 = new JLabel("Sampling Rate");
    pNames2.add(name7);
    JLabel name8 = new JLabel("Audio Buffer");
    pNames2.add(name8);

    p12.add(pNames2);

    p12.add(Box.createHorizontalGlue());

    JPanel pValues2 = new JPanel();
    pValues2.setLayout(new BoxLayout(pValues2, BoxLayout.Y_AXIS));
    pValues2.setPreferredSize(new Dimension(130, 30));
    JLabel value7 = new JLabel(control.getSamplingRate().toString());
    pValues2.add(value7);
    JLabel value8 = new JLabel(control.getFifoSize().toString());
    pValues2.add(value8);
    
    p12.add(pValues2);

    p1.add(p12);

    getContentPane().add("Center", p1);
    
    //Create bottom row.
    JPanel p2 = new JPanel();
    p2.setPreferredSize(new Dimension(200, 35));
    p2.setLayout(new FlowLayout(FlowLayout.CENTER));
    JButton okButton = new JButton("OK");
    okButton.addActionListener(this);
    p2.add(okButton);
    getContentPane().add("South", p2);

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










