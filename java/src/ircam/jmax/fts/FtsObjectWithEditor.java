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

package ircam.jmax.fts;

import ircam.fts.client.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.*;
import java.awt.*;

// import javax.swing.*;
import javax.swing.SwingUtilities;

import java.lang.*;
import java.io.*;

public abstract class FtsObjectWithEditor extends FtsUndoableObject {
  
  static{
    FtsObject.registerMessageHandler( FtsObjectWithEditor.class, FtsSymbol.get("createEditor"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
    {
        ((FtsObjectWithEditor)obj).createEditor();
    }
    });    
    FtsObject.registerMessageHandler( FtsObjectWithEditor.class, FtsSymbol.get("openEditor"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
        ((FtsObjectWithEditor)obj).openEditor( args.getLength(), args.getAtoms());
      }
    });
    FtsObject.registerMessageHandler( FtsObjectWithEditor.class, FtsSymbol.get("destroyEditor"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
        ((FtsObjectWithEditor)obj).destroyEditor();
      }
    });  
    FtsObject.registerMessageHandler( FtsObjectWithEditor.class, FtsSymbol.get("closeEditor"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
        ((FtsObjectWithEditor)obj).destroyEditor();
      }
    });  
  }
  
  public FtsObjectWithEditor(FtsServer server, FtsObject parent, int id, String className, FtsAtom[] args, int offset, int length)
{
    super(server, parent, id, className, args, offset, length);
}

public FtsObjectWithEditor(FtsServer server, FtsObject parent, int id, String className, String description)
{
  super(server, parent, id, className, description);
}

public FtsObjectWithEditor(FtsServer server, FtsObject parent, int id)
{
  super(server, parent, id);
}

public  FtsObjectWithEditor(FtsServer server, FtsObject parent, FtsSymbol className) throws IOException
{
  super(server, parent, className);
}

public Frame getEditorFrame()
{
  return editorFrame;
}

public void setEditorFrame(Frame frame)
{
  editorFrame = frame;
}

public void disposeEditor()
{
  if(editorFrame!= null)
  {
    hideEditor();
    SwingUtilities.invokeLater(new Runnable() {
	    public void run()
    { 
	       if(editorFrame!= null)
           editorFrame.dispose();
	       editorFrame = null;
    }
	  }); 
  }
}
public void showEditor()
{
  if(editorFrame != null)
  {
    if(!editorFrame.isVisible())
    {
      editorFrame.setVisible(true);
      MaxWindowManager.getWindowManager().addWindow(editorFrame);
    }
    else
    {
      SwingUtilities.invokeLater(new Runnable() {
        public void run()
        { 
          if(editorFrame.getState()==Frame.ICONIFIED) 
            editorFrame.setState(Frame.NORMAL);
          editorFrame.toFront();
        }
      });
    }
  }
}
public void hideEditor()
{
  editorFrame.setVisible(false);
  MaxWindowManager.getWindowManager().removeWindow(editorFrame);
}

public void closeEditor()
{
  if(getEditorFrame() != null)	    
    hideEditor();
}

void releaseData()
{
  requestDestroyEditor();
}

public void requestDestroyEditor()
{
  try{
    send(FtsSymbol.get("destroyEditor"));
  }
  catch(IOException e)
{
    System.err.println("FtsObjectWithEditor: I/O Error sending destroyEditor Message!");
    e.printStackTrace(); 
}
}

public void requestOpenEditor()
{
  try{
    send(FtsSymbol.get("openEditor"));
  }
  catch(IOException e)
{
    System.err.println("FtsObjectWithEditor: I/O Error sending openEditor Message!");
    e.printStackTrace(); 
}
}

public void openEditor(int argc, FtsAtom[] argv)
{  
  createEditor();  
  showEditor();
  FtsObject.requestResetGui();
}

public abstract void createEditor();
public abstract void destroyEditor();

private transient Frame editorFrame = null;
}
