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
package ircam.jmax.fts;

import java.io.*;
import java.net.*;

/**
 * The datagram socket connection.
 * Implement a specialed connection using a datagram socket 
 * for communicating with FTS; start FTS thru a rsh command,
 * so it will work only on unix machines and the like.
 */

class FtsDatagramStream extends FtsStream
{
  int sequence = -1;
  final static private int max_packet_size = 2048;
  DatagramSocket socket = null;
  int receivedPacketSize;
  byte in_data[] = new byte[max_packet_size];
  byte out_data[] = new byte[max_packet_size];
  int in_fill_p = -1;           // point to the next char to read in in_data, -1 if no packet read yet.
  int out_fill_p = 0;		// point to the next free char in out_data
  DatagramPacket out_packet = new DatagramPacket(out_data, out_data.length);
  DatagramPacket in_packet;

  String host;
  String path;
  String ftsName;
  Process proc;

  FtsDatagramStream(String host, String path, String ftsName)
  {
    super(host);

    String command;

    this.host = host;
    this.path = path;
    this.ftsName = ftsName;

    try
      {
	this.socket = new DatagramSocket();// look for a free port
      }
    catch (java.io.IOException e)
      {
	System.out.println("Error while opening server socket " + e);
      }

    try
      {
	if (host.equals(InetAddress.getLocalHost().getHostName()))
	  {
	    command = (path + "/" + ftsName + ( Fts.getNoRealTime() ? " -norealtime" : "")
		       + " udp " + InetAddress.getLocalHost().getHostAddress() + ":" + socket.getLocalPort()) ;
	  }
	else
	  {
	    command = ("rsh " + host + " " + path + "/" + ftsName + ( Fts.getNoRealTime() ? " -norealtime" : "")
		       + " udp " + InetAddress.getLocalHost().getHostAddress() + ":" + socket.getLocalPort()) ;
	  }
      }
    catch (UnknownHostException e)
      {
	System.out.println("Cannot find local host");
	return;
      }


    // Run FTS remotely

    try
      {
	proc = Runtime.getRuntime().exec(command);
      }
    catch (IOException e)
      {
	System.out.println("Cannot exec command: " + command);
	return;
      }


    // Wait a first answerback packet 
    // Its content is ignore, is used for startup syncronization
    // and to get the fts port number and address

    try
      {
	FtsErrorStreamer.startFtsErrorStreamer(proc.getErrorStream());
	in_packet  = new DatagramPacket(in_data , in_data.length);
	socket.receive(in_packet);
	out_packet.setAddress(in_packet.getAddress());
	out_packet.setPort(in_packet.getPort());
      }
    catch (IOException e)
      {
	System.out.println("I/O error during accept on server socket");
	return;
      }    
  }

  void close()
  {
    FtsErrorStreamer.stopFtsErrorStreamer();
    socket.close();
    in_packet = null;
    out_packet = null;

    proc.destroy();
  }

  boolean isOpen()
  {
    return (socket != null);
  }


  /** Method to send a char; since we can use datagram sockets or other
    means I/O is not necessarly done thru streams */

  protected void write(int data) throws java.io.IOException
  {
    out_data[out_fill_p++] = (byte) data;

    if (out_fill_p >= max_packet_size)
      flush();
  }

  /** Method to receive a char; since we can use datagram sockets or other
    means I/O is not necessarly done thru streams */

  protected int read() throws java.io.IOException
  {
    int c;

    if (in_fill_p == -1)
      {
	in_packet  = new DatagramPacket(in_data , in_data.length);

	socket.receive(in_packet);

	if (sequence < 0)
	  sequence = in_data[0];
	else
	  {
	    sequence = (sequence + 1) % 128;

	    if (sequence != in_data[0])
	      {
		System.out.println("UDP: Sequence received " + in_data[0] + " sequence expected " + sequence);
		System.out.println("UDP: unreliable network, use a tcp connection !");
		sequence = in_data[0];
	      }
	  }

	receivedPacketSize = ((in_data[1] < 0 ? in_data[1] + 256 : in_data[1]) * 256 +
			      ((in_data[2] < 0 ? in_data[2] + 256 : in_data[2])));

	in_fill_p = 3;
      }

    c = in_data[in_fill_p++];

    if (in_fill_p >=  receivedPacketSize)
      in_fill_p = -1;

    return c;
  }

  /** Method to Ask for an explicit output flush ; since we
    can use datagram sockets or other means I/O is not necessarly done
    thru streams */

  void flush() throws java.io.IOException
  {
    out_packet.setLength(out_fill_p);

    try
      {
	socket.send(out_packet);
      }
    finally
      {
	out_fill_p = 0;
      }
  }
}




