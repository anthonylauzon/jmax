package ircam.jmax.fts;
import java.util.*;

/**
 * This class provide a registration service for TCL FTS templates.
 * The class is used thru the TCL Command <b>template</b>.
 *
 * @see FtsTemplateCmd
 */


public class FtsTemplateTable
{
  static Hashtable templateTable = new Hashtable(255, 0.99f);

  static public void add(String name, String proc)
  {
    templateTable.put(name, proc);
  }

  static public boolean exists(String name)
  {
    return templateTable.containsKey(name);
  }

  static String getProc(String name)
  {
    return (String) templateTable.get(name);
  }
}
