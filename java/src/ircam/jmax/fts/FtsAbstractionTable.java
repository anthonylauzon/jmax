package ircam.jmax.fts;
import java.util.*;

/**
 * This class provide a registration service for .abs abstractions.
 * The class is used thru the TCL Command <b>abstraction</b>.
 *
 * @see FtsAbstrationCmd
 */

public class FtsAbstractionTable
{
  static Hashtable abstractionTable = new Hashtable(256, 0.99f);

  static public void add(String name, String filename)
  {
    abstractionTable.put(name, filename);
  }

  static public boolean exists(String name)
  {
    return abstractionTable.containsKey(name);
  }

  static String getFilename(String name)
  {
    return (String) abstractionTable.get(name);
  }
}
