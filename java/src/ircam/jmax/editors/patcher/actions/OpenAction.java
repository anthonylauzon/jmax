//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

package ircam.jmax.editors.patcher.actions;

import java.io.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.patcher.*;

public class OpenAction extends MenuAction
{
  public void doAction(ErmesSketchWindow editor)
  {
    JFrame frame = editor;
    File file = MaxFileChooser.chooseFileToOpen(frame);

    if (file != null)
      {
	Cursor temp = frame.getCursor();

	try
	  {
	    MaxDocument document;

	    frame.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
	    document = Mda.loadDocument(editor.getFts(), file);
	
	    try
	      {
		if (document.getDocumentType().isEditable())
		  document.edit();
	      }
	    catch (MaxDocumentException ex)
	      {
		// Ignore MaxDocumentException exception in running the editor
		// May be an hack, may be is ok; move this stuff to an action
		// handler !!
	      }

	    frame.setCursor(temp);
	  }
	catch (MaxDocumentException e)
	  {
	    frame.setCursor(temp);
	    new ErrorDialog(frame, e.toString());
	  }
      }
  }
}




