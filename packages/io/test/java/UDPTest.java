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

import java.io.*;
import java.net.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

public class UDPTest extends JFrame {

  private UDPPort udpPort;
  byte[] buffer;

  public UDPTest( String host, int port)
  {
    super( "UDPTest on " + host + ":" + port);

    addWindowListener( new WindowAdapter() {
      public void windowClosing( WindowEvent e) 
	{
	  System.exit(0);
	}
    });

    buffer = new byte[1];

    udpPort = new UDPPort( host, port);

    JSlider slider = new JSlider( JSlider.VERTICAL, 0, 127, 0);

    slider.addChangeListener( new ChangeListener() {
      public void stateChanged(ChangeEvent e)
	{
	  buffer[0] = (byte)((JSlider)e.getSource()).getValue();
	  udpPort.send( buffer, 1);
	}
    });

    slider.setBorder( new EmptyBorder( 10, 10, 10, 10));

    setSize( 200, 200);

    getContentPane().setLayout( new BoxLayout( getContentPane(), BoxLayout.X_AXIS));

    getContentPane().add( slider);
    getContentPane().add( Box.createHorizontalGlue());

    validate();
    setVisible( true);
  }

  public static void main(String args[])
  {
    String host = null;
    int port = 0;

    try
      {
	host = args[0];
	port = Integer.parseInt( args[1]);
      }
    catch (Exception e)
      {
	System.out.println( "[java] arguments expected: <host> <port>" );
	System.exit(1);
      }

    new UDPTest( host, port);
  }
}

