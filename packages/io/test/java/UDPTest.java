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

