package ircam.jmax.fts;

import java.io.*;

/**
 * SubProcess Server class.
 * This class implement
 * an FtsServer using a FtsSubProcessPort, i.e.
 * a connection where FTS is started as a local subprocess
 * and pipes are used for the physical connections.
 *
 * @see FtsSubProcessPort
 */

public class FtsSubProcessServer extends FtsServer
{
  /** Create a new FtsSubProcessServer. */

  public FtsSubProcessServer()
  {
    super("fts", new FtsSubProcessPort("fts"));
  }

  /** 
   * Create a new FtsSubProcessServer.
   *
   * @param stream the stream to use as post stream.
   */

  public FtsSubProcessServer(PrintStream stream)
  {
    super("fts", new FtsSubProcessPort("fts"), stream);
  }
}
