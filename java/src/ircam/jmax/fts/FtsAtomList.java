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

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.ftsclient.*;

/** 
 * The Java Implementation for the atom list remote data class.
 */

public class FtsAtomList extends FtsObject
{
  static
  {
      FtsObject.registerMessageHandler( FtsAtomList.class, FtsSymbol.get("setValues"), new FtsMessageHandler(){
	      public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	      {
		  ((FtsAtomList)obj).setValues(argc, argv);
	      }
	  });
      FtsObject.registerMessageHandler( FtsAtomList.class, FtsSymbol.get("setName"), new FtsMessageHandler(){
	      public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	      {
		  ((FtsAtomList)obj).setName(argv[0].stringValue);
	      }
	  });
  }

  String name; // the list name (read only)
  MaxVector values = new MaxVector();
  MaxVector listeners = new MaxVector();

  protected FtsArgs args = new FtsArgs();

  public FtsAtomList(FtsServer server, FtsObject parent, FtsSymbol classname, FtsArgs args) throws IOException
  { 
    super(server, parent, classname, args);
  }
  public FtsAtomList(FtsServer server, FtsObject parent, FtsSymbol classname) throws IOException
  { 
    super(server, parent, classname);
  }

  /** Return the size of the atom list */

  public int getSize()
  {
    return values.size();
  }

  /** Get the content of the atom list as a MaxVector */

  public MaxVector getValues()
  {
    return values;
  }

  /* Access the name */
  public String getName()
  {
    return name;
  }

  /** Get the content of the atom list as text. */

  public String getValuesAsText()
  {
    return FtsParse.unparseDescription(values);
  }


  /** Set the content of the atom list as text. */
  
  public void setValuesAsText(String value)
  {
    values.removeAllElements();
    FtsParse.parseAtoms(value, values);
    changed();
  }


  /** Update the content of the atom list from the server. */

  public void forceUpdate()
  {
      try{
	  send( FtsSymbol.get("atomlist_update"));
      }
      catch(IOException e)
	  {
	      System.err.println("FtsAtomList: I/O Error sending update Message!");
	      e.printStackTrace(); 
	  }
  }


  /** 
   * Send the content of the atom list to the server.
   */

  public void changed()
  {
      args.clear();
      for(int i = 0; i < values.size(); i++)
	  args.add(values.elementAt(i));
      
      try{
	  send( FtsSymbol.get("atomlist_set"), args);
      }
      catch(IOException e)
	  {
	     System.err.println("FtsAtomList: I/O Error sending set Message!");
	     e.printStackTrace(); 
	  }
  }

  /* Server call-back */

  public void setValues(int nArgs , FtsAtom args[])
  {
    values.removeAllElements();
    
    for(int i = 0; i < nArgs; i++)	
	values.addElement(args[i].getValue());

    fireContentChanged();
  }
  public void setName(String newName)
  {
      name = newName;
  }
    
  void fireContentChanged()
  {
      for(Enumeration e = listeners.elements(); e.hasMoreElements();)
	  ((FtsAtomListListener)e.nextElement()).contentChanged();
  }
  public void addFtsAtomListListener(FtsAtomListListener listener)
  {
      listeners.addElement(listener);
  }
  public void removeFtsAtomListListener(FtsAtomListListener listener)
  {
      listeners.removeElement(listener);
  }
}


