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


import java.io.*;
import java.net.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

import ircam.jmax.protocol.*;

public class UDPMessageTest extends JFrame {

  private UDPPort udpPort;
  ProtocolEncoder encoder;

  class MessageSlider extends JPanel {

    class MessageSliderChangeListener implements ChangeListener {
      public void stateChanged(ChangeEvent e)
      {
	encoder.start();
	encoder.putString( ((JSlider)e.getSource()).getName());
	encoder.putInt( ((JSlider)e.getSource()).getValue());
	encoder.end();

	udpPort.send( encoder.getMess(), encoder.getSize());
      }
    }

    MessageSlider( String name)
    {
      setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));

      JSlider slider = new JSlider( JSlider.VERTICAL, 0, 127, 0);

      slider.addChangeListener( new MessageSliderChangeListener());
      slider.setName( name);

      add( slider);
      add( Box.createVerticalStrut( 10));
      add( new JLabel( name, SwingConstants.LEFT));
      //setBorder( new TitledBorder( new EmptyBorder( 10, 10, 10, 10), name));
      setBorder( LineBorder.createGrayLineBorder());
    }
  }

  public UDPMessageTest( String host, int port, String names[])
  {
    super( "UDPMessageTest on " + host + ":" + port);

    addWindowListener( new WindowAdapter() {
      public void windowClosing( WindowEvent e) 
	{
	  System.exit(0);
	}
    });

    udpPort = new UDPPort( host, port);
    encoder = new ProtocolEncoder();

    setSize( 200, 200);

    getContentPane().setLayout( new BoxLayout( getContentPane(), BoxLayout.X_AXIS));

    for ( int i = 0; i < names.length; i++)
      {
	getContentPane().add( new MessageSlider( names[i]));
	getContentPane().add( Box.createHorizontalGlue());
      }

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

    String names[] = new String[args.length-2];

    System.arraycopy( args, 2, names, 0, args.length-2);

    new UDPMessageTest( host, port, names);
  }
}

