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

  static public boolean exists(String className)
  {
    String realName;

    if (className.endsWith(".pat"))
      realName = className.substring(0, className.lastIndexOf(".pat"));
    else if (className.endsWith(".abs"))
      realName = className.substring(0, className.lastIndexOf(".abs"));
    else
      realName = className;

    return abstractionTable.containsKey(realName);
  }

  static String getFilename(String className)
  {
    String realName;

    if (className.endsWith(".pat"))
      realName = className.substring(0, className.lastIndexOf(".pat"));
    else if (className.endsWith(".abs"))
      realName = className.substring(0, className.lastIndexOf(".abs"));
    else
      realName = className;

    return (String) abstractionTable.get(realName);
  }
}




