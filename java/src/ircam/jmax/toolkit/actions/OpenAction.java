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

package ircam.jmax.toolkit.actions;

import java.io.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

public class OpenAction extends EditorAction
{
  Frame frame;
    Fts fts;
  
  public void doAction(EditorContainer container)
  {
    File file = MaxFileChooser.chooseFileToOpen(container.getFrame());
    frame = container.getFrame();
    fts = container.getEditor().getFts();

    if (file != null)
      {
	try
	  {
	    fts.fireAtomicAction(true);

	    MaxDocument document = Mda.loadDocument(fts, file);
	    try
	      {		
		  frame.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));  

		  if (document.getDocumentType().isEditable())
		      document.edit();

		  ((FtsPatcherObject)document.getRootData()).requestStopWaiting(new FtsActionListener(){
			  public void ftsActionDone()
			  {
			      frame.setCursor(Cursor.getDefaultCursor());
			      fts.fireAtomicAction(false);
			  }
		    });
	      }
	    catch (MaxDocumentException ex)
	      {
		  frame.setCursor(Cursor.getDefaultCursor());
		  // Ignore MaxDocumentException exception in running the editor
		  // May be an hack, may be is ok; move this stuff to an action
		  // handler !!
	      }
	  }
	catch (MaxDocumentException e)
	  {
	    frame.setCursor(Cursor.getDefaultCursor());
	    JOptionPane.showMessageDialog(container.getFrame(), e.toString(), 
					  "Error", JOptionPane.ERROR_MESSAGE);
	  }
      }
  }
}




