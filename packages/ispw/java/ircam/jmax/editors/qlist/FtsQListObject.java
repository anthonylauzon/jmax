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

package ircam.jmax.editors.qlist;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;

import ircam.jmax.ispw.*;

import java.io.*;
import java.util.*;

/**
 * A concrete implementation of the SequenceDataModel,
 * this class represents a model of a set of tracks.
 */
public class FtsQListObject extends FtsObjectWithEditor {

  static{
    FtsObject.registerMessageHandler( FtsQListObject.class, FtsSymbol.get("setAtomList"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  FtsAtomList atomList = new FtsAtomList( JMaxApplication.getFtsServer(), null, args.getInt( 0));
	  ((FtsQListObject)obj).setAtomList( atomList);
	  
	}
      });
  }

  /**
   * constructor.
   */
  public FtsQListObject(FtsServer server, FtsObject parent, int objId, String classname, FtsAtom args[], int offset, int length)
  {
    super(server, parent, objId, classname, args, offset, length);		
  }

  public void requestOpenEditor()
  {
    requestUpload();
    super.requestOpenEditor();
  }

  public void requestUpload()
  {
    try{
      send(FtsSymbol.get("upload"));
    }
    catch(IOException e)
      {
	System.err.println("FtsQlistObject: I/O Error sending upload Message!");
	e.printStackTrace(); 
      }
  }

  //////////////////////////////////////////////////////////////////////////////////////
  //// MESSAGES called from fts.
  //////////////////////////////////////////////////////////////////////////////////////

  public void openEditor(int argc, FtsAtom[] argv)
  {
    if(getEditorFrame() == null)	    
      setEditorFrame( new QListWindow(this));
	
    showEditor();
  }

  public void destroyEditor()
  {
    disposeEditor();
  }

  public void setAtomList(FtsAtomList l)
  {
    if(list != null)
      {
	for(Enumeration e = list.getFtsAtomListListeners(); e.hasMoreElements();)
	  l.addFtsAtomListListener( (FtsAtomListListener) e.nextElement());
      }
    list = l;    
  }

  public FtsAtomList getAtomList()
  {
    return list;
  }

  FtsAtomList list;
}
