package ircam.jmax.fts;

import java.io.*;

/**
 * Datagram Server class.
 * This class implement
 * an FtsServer using a FtsDatagramPort, i.e.
 * a connection based
 *
 * @see FtsDatagramPort
 */

public class FtsRexecDatagramServer extends FtsServer
{
  /** Create a new FtsDatagramServer. */

  public FtsRexecDatagramServer(String name)
  {
    super(name, new FtsRexecDatagramPort(name));
  }
}
