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

class UDPPort  {
  UDPPort( String host, int port)
  {
    this.port = port;

    try
      {
	addr = InetAddress.getByName(host);
      }
    catch (UnknownHostException e)
      {
	System.err.println("[java] Don't know about host " + host );
	System.exit(0);
      }

    try
      {
	socket = new DatagramSocket();
      }
    catch (IOException e)
      {
	System.err.println("[java] Couldn't open socket");
	System.exit(0);
      }
  }

  void send( byte[] buffer, int length)
  {
    DatagramPacket packet = new DatagramPacket( buffer, length, addr, port);

    try
      {
	socket.send( packet);
      }
    catch (java.io.IOException e)
      {
	System.out.println( "Got exception [" + e + "]");
	System.exit(1);
      }
  }

  DatagramSocket socket;
  InetAddress addr;
  int port;
}

