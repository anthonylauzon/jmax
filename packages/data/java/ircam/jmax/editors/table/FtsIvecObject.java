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

package ircam.jmax.editors.table;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

import java.awt.datatransfer.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

/**
 * A concrete implementation of the SequenceDataModel,
 * this class represents a model of a set of tracks.
 */
public class FtsIvecObject extends FtsObjectWithEditor 
{
  static
  {
    FtsObject.registerMessageHandler( FtsIvecObject.class, FtsSymbol.get("editor"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{	  
	  ((FtsIvecObject)obj).setFtsEditor( args.getInt( 0));
	}
      });
  }

  /**
   * constructor.
   */
  public FtsIvecObject(FtsServer server, FtsObject parent, int objId, String classname, FtsAtom args[], int offset, int length)
  {
    super(server, parent, objId, classname, args, offset, length);
  }

  //////////////////////////////////////////////////////////////////////////////////////
  //// MESSAGES called from fts.
  //////////////////////////////////////////////////////////////////////////////////////

  public void setFtsEditor(int id)
  {
    this.ftsEditor = new FtsTableObject( getServer(), this, id);
  }

   public FtsTableObject getFtsEditor()
  {
    return ftsEditor;
  }
  
  public void openEditor(int argc, FtsAtom[] argv)
  {
    if(getEditorFrame() == null)
      setEditorFrame( new Tabler( this, ftsEditor));
    
    showEditor();
  }
  
  public void destroyEditor()
  {
    disposeEditor();
  }

  FtsTableObject ftsEditor;
}











