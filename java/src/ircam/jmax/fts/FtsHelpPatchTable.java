//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
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
  static Hashtable helpSummaryTable = new Hashtable();

  static public void add(String className, String patch)
  {
    helpTable.put(className, patch);
  }

  static public void addSummary(String name, String patch)
  {
    helpSummaryTable.put(name, patch);
  }

  static public boolean exists(String className)
  {
    return helpTable.containsKey(className);
  }

  static public boolean summaryExists(String name)
  {
    return helpSummaryTable.containsKey(name);
  }

  static String getHelpPatch(String className)
  {
    return (String) helpTable.get(className);
  }

  static String getHelpSummaryPatch(String className)
  {
    return (String) helpSummaryTable.get(className);
  }

  static public boolean openHelpPatch(FtsObject obj)
  {
    if (exists(obj.getClassName()))
      {
	try
	  {
	    File file = new File(getHelpPatch(obj.getClassName()));
	    MaxDocument document;

	    document = Mda.loadDocument(file);
	    document.edit();
	  }
	catch (MaxDocumentException e)
	  {
	    return false;
	  }

	return true;
      }
    else
      return false;
  }

  static public boolean openHelpSummary(String name)
  {
    if (summaryExists(name))
      {
	try
	  {
	    File file = new File(getHelpSummaryPatch(name));
	    MaxDocument document;

	    document = Mda.loadDocument(file);
	    document.edit();
	  }
	catch (MaxDocumentException e)
	  {
	    return false;
	  }

	return true;
      }
    else
      return false;
  }

  static public Enumeration getSummaries()
  {
    return helpSummaryTable.keys();
  }
}



