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

public class FtsSocketServer extends FtsServer
{
  /** Create a new FtsSocketServer. */

  public FtsSocketServer(String name, int port)
  {
    super(name + ":" + port, new FtsSocketPort(name, port));
  }
}
