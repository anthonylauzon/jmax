package ircam.jmax.fts;

import java.util.*;
import java.io.*;

import ircam.jmax.mda.*;

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

  static public boolean openHelpPatch(FtsObject obj)
  {
    if (exists(obj.getClassName()))
      {
	try
	  {
	    MaxDataSource source = MaxDataSource.makeDataSource(getHelpPatch(obj.getClassName()));
	    MaxData data;

	    data = MaxDataHandler.loadDataInstance(source);
	    data.edit();
	  }
	catch (MaxDataException e)
	  {
	    return false;
	  }
	catch (java.net.MalformedURLException e)
	  {
	    return false;
	  }

	return true;
      }
    else
      return false;
  }
}



