//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
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

