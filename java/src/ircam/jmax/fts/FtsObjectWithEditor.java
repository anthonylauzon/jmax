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

import ircam.ftsclient.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.*;
import javax.swing.undo.*;
import javax.swing.event.*;
import java.awt.*;
import javax.swing.*;

import java.lang.*;
import java.io.*;

/**
 * An fts remote data that offers a built-in undo support.
 * 
 */
public abstract class FtsObjectWithEditor extends FtsUndoableObject {

    static{
	FtsObject.registerMessageHandler( FtsObjectWithEditor.class, FtsSymbol.get("openEditor"), new FtsMessageHandler(){
		public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
		{
		    ((FtsObjectWithEditor)obj).openEditor(argc, argv);
		}
	    });
	FtsObject.registerMessageHandler( FtsObjectWithEditor.class, FtsSymbol.get("destroyEditor"), new FtsMessageHandler(){
		public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
		{
		    ((FtsObjectWithEditor)obj).destroyEditor();
		}
	    });  
    }
  
  public FtsObjectWithEditor(FtsServer server, FtsObject parent, FtsSymbol className, int nArgs, FtsAtom[] args, int id)
  {
    super(server, parent, className, nArgs, args, id);
  }

  public FtsObjectWithEditor(FtsServer server, FtsObject parent, FtsSymbol ftsClassName, FtsArgs args) throws IOException
  {
      super(server, parent, ftsClassName, args);
  }
  
  public FtsObjectWithEditor(FtsServer server, FtsObject parent, FtsSymbol ftsClassName) throws IOException
  {
      super(server, parent, ftsClassName);
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
			    editorFrame.dispose();
			    editorFrame = null;
			}
		    }); 
	    }
  }
  public void showEditor()
  {
      if(editorFrame!=null)
	  if (! editorFrame.isVisible())
	  {
	      editorFrame.setVisible(true);
	      MaxWindowManager.getWindowManager().addWindow(editorFrame);
	  }   
      
      if(editorFrame.getState()==Frame.ICONIFIED) editorFrame.setState(Frame.NORMAL);
      editorFrame.toFront();
  }

  public void hideEditor()
  {
      editorFrame.setVisible(false);
      MaxWindowManager.getWindowManager().removeWindow(editorFrame);
  }

  public void closeEditor(int nArgs, FtsAtom args[])
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

  public abstract void openEditor(int argc, FtsAtom[] argv);
  public abstract void destroyEditor();

  private Frame editorFrame = null;
}













