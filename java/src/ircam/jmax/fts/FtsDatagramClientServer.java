package ircam.jmax.fts;

import java.io.*;

/**
 * Socket Server class.
 * This class implement
 * an FtsServer using a FtsSocketPort, i.e.
 * a connection where the user environment work as a 
 * client (in the socket meaning of the term), and FTS
 * as a server.
 *
 * @see FtsSocketPort
 */

public class FtsDatagramClientServer extends FtsServer
{
  /** Create a new FtsDatagramClientServer. */

  public FtsDatagramClientServer(String name, int port)
  {
    super(name + ":" + port, new FtsDatagramClientPort(name, port));
  }
}
