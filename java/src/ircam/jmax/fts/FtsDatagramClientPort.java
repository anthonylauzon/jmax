package ircam.jmax.fts;

import java.io.*;
import java.net.*;

/**
 * The datagram socket connection.
 * Implement a specialed connection using a datagram socket 
 * for communicating with FTS; start FTS thru a rsh command,
 * so it will work only on unix machines and the like.
 */

/* 
 * Da fare: passare a fts udpsocket come argomento, e l'indirizzo incluso
 * il port locale; poi, aggiungere variabili per tenere l'indirizzo del
 * server fts, e riempirlo al primo pacchetto.
 * Poi, fare il pacchettamento/ripacchettamento lato ricezione/trasmissione
 * dati
 */

class FtsDatagramClientPort extends FtsPort
{
  final static private int max_packet_size = 256;
  DatagramSocket socket = null;
  byte in_data[]  = new byte[max_packet_size]; 
  byte out_data[] = new byte[max_packet_size];
  int in_fill_p = -1;           // point to the next char to read in in_data, -1 if no packet read yet.
  int out_fill_p = 0;		// point to the next free char in out_data
  DatagramPacket out_packet = new DatagramPacket(out_data, out_data.length);
  DatagramPacket in_packet;

  String host;
  String path;
  String ftsName;
  int port;

  FtsDatagramClientPort(String host, String path, String ftsName, int port)
  {
    super(host);

    String command;

    this.host = host;
    this.port = port;
    this.path = path;
    this.ftsName = ftsName;

    try
      {
	this.socket = new DatagramSocket(port);// look for a free port
      }
    catch (java.io.IOException e)
      {
	System.out.println("Error while opening server socket " + e);
      }

    // FTS Must be started by hand in this case, *after* jmax
    // with the command fts udp <host>:<port>
    // Wait a first answerback packet 
    // Its content is ignore, is used for startup syncronization
    // and to get the fts port number and address

    try
      {
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
    socket.close();
    in_packet = null;
    out_packet = null;
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
	in_fill_p = 0;
      }

    c = in_data[in_fill_p++];

    if (in_fill_p >= in_packet.getLength())
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





