//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.fts;

import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.fts.client.*;

/**
 * This class provide a registration service for help Patches.
 * The class is filled thru the TCL Command <b>helpPatch</b>.
 * Note that there is a single help patch data base for all the servers.
 *
 * @see FtsHelpPatchCmd
 */


public class FtsHelpPatchTable
{
  static Hashtable helpTable = new Hashtable(256, 0.99f);
  static Hashtable helpSummaryTable = new Hashtable();
  static Vector helpSummaryVector = new Vector();

  /** Add an help patch for an fts object class */

  static public void add(String className, String patch)
  {
    helpTable.put(className, patch);
  }

  /** Add an help summary patch */

  static public void addSummary(String name, String patch)
  {
    helpSummaryTable.put(name, patch);
    helpSummaryVector.addElement(name);
  }

  /** Check if the help patch for a given fts object class exists */

  static public boolean exists(String className)
  {
    return helpTable.containsKey(className);
  }

  /** Check if a named help patch summary exists */

  static public boolean summaryExists(String name)
  {
    return helpSummaryTable.containsKey(name);
  }

  /** Get the help patch filename for a given fts object class exists */

  static String getHelpPatch(String className)
  {
    return (String) helpTable.get(className);
  }


  /** Get a named help patch summary filename */

  static String getHelpSummaryPatch(String className)
  {
    return (String) helpSummaryTable.get(className);
  }

  /** Open an help patch for a given fts object */

  static public boolean openHelpPatch(FtsObject obj)
  {
      /*if (exists(obj.getClassName()))
	{
	try
	{
	File file = new File(getHelpPatch(obj.getClassName()));
	MaxDocument document;
	    
	document = Mda.loadDocument(obj.getFts(), file);
	document.edit();
	}
	catch (MaxDocumentException e)
	{
	return false;
	}

	return true;
	}
	else*/
      return false;
  }


  /** Open an help summary patch */

  static public void openHelpSummary( String name)
  {
    if (summaryExists(name))
      {
	String file = getHelpSummaryPatch(name);
	try
	  {
	    JMaxApplication.getFtsServer().getRoot().load( file);
	  }
	catch(IOException e)
	  {
	    System.err.println("[FtsHelpPatchTable]: I/O error opening Help Summary file "+file);
	  }
      }
  }

  /** Get an enumeration of the available summaries */

  static public Enumeration getSummaries()
  {
      return helpSummaryVector.elements();
  }
  static public int getNumSummaries()
  {
      return helpSummaryVector.size();
  }
}







