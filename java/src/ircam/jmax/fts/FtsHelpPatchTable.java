package ircam.jmax.fts;

import java.util.*;
import java.io.*;

/**
 * This class provide a registration service for help Patches.
 * The class is filled thru the TCL Command <b>helpPatch</b>.
 *
 * @see FtsHelpPatchCmd
 */


public class FtsHelpPatchTable
{
  static Hashtable helpTable = new Hashtable(256, 0.99f);

  static public void add(String className, String patch)
  {
    helpTable.put(className, patch);
  }

  static public boolean exists(String className)
  {
    return helpTable.containsKey(className);
  }

  static String getHelpPatch(String className)
  {
    return (String) helpTable.get(className);
  }

  /** Access to the help patch for an object. */

  public static File getHelpPatch(FtsObject obj)
  {
    if (exists(obj.getClassName()))
      return new File(getHelpPatch(obj.getClassName()));
    else
      return null;
  }
}



