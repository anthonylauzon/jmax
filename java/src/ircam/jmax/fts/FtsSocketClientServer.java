package ircam.jmax.fts;
import java.io.*;

/**
 * Socket Client server class.
 * This class implement
 * an FtsServer using a FtsSocketServerPort, i.e.
 * a connection where the user environment work as a 
 * server (in the socket meaning of the term), and FTS
 * as a client.
 *
 * @see FtsSocketServerPort
 */

public class FtsSocketClientServer extends FtsServer
{
  /** Create a new FtsSocketClientServer. */

  public FtsSocketClientServer(String name)
  {
    super(name, new FtsSocketServerPort(name));
  }

  /**
   * Create a new FtsSocketClientServer.
   * 
   * @param stream the stream to use as post stream.
   */

  public FtsSocketClientServer(String name, PrintStream stream)
  {
    super(name, new FtsSocketServerPort(name), stream);
  }
}
