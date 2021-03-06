//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.mat;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

import java.awt.datatransfer.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import java.awt.*;

/**
 * A concrete implementation of the SequenceDataModel,
 * this class represents a model of a set of tracks.
 */
public class FtsFmatObject extends FtsObjectWithEditor
{
  static
  {
    FtsObject.registerMessageHandler( FtsFmatObject.class, FtsSymbol.get("editor"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
        ((FtsFmatObject)obj).setFtsEditor( args.getInt( 0));
      }
    });
  }

  /**
   * constructor.
   */
  public FtsFmatObject(FtsServer server, FtsObject parent, int objId, String classname, FtsAtom args[], int offset, int length)
  {
    super(server, parent, objId, classname, args, offset, length);     
  }

  //////////////////////////////////////////////////////////////////////////////////////
  //// MESSAGES called from the server
  //////////////////////////////////////////////////////////////////////////////////////
  FtsMatEditorObject ftsEditor;
  
  public void setFtsEditor(int id)
  {
    this.ftsEditor = new FtsMatEditorObject( getServer(), this, id);
  }

  public FtsMatEditorObject getFtsEditor()
  {
    return ftsEditor;
  }

  public void createEditor()
  {
    if(getEditorFrame() == null)
      setEditorFrame( new MatWindow(this, ftsEditor));
  }

  public void reinitEditorFrame()
  {
    setEditorFrame( new MatWindow((MatWindow)getEditorFrame()));
  }
    
  boolean firstTime = true;
  boolean doing_open_editor = false;
  public void openEditor(int argc, FtsAtom[] argv)
  {      
    if(getEditorFrame() == null)
    {
      createEditor();// rest moved in endUpload   
      firstTime = true;
    }
    else
      firstTime = false;
    
    doing_open_editor = true;
  }

  public boolean doingOpenEditor()
  {
    return doing_open_editor;
  }

  public void destroyEditor()
  {
    disposeEditor();
    System.gc();
  }
    
  public void showEditor()
  {
    showEditor(firstTime);
    doing_open_editor = false;
  }
  
  public Dimension getDefaultSize()
  {
    return defaultSize;
  }
  
  public void nameChanged( String name)
  {
    super.nameChanged( name);
    if(ftsEditor != null)
      ftsEditor.nameChanged( name);
  }
  
  public final static int FMAT_DEFAULT_WIDTH  = 370;
  public final static int FMAT_DEFAULT_HEIGHT = 250;
  static Dimension defaultSize = new Dimension(FMAT_DEFAULT_WIDTH, FMAT_DEFAULT_HEIGHT);
}











